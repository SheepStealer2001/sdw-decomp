// Load the recovered structs/enums into Ghidra and type `this` for class methods.
// Structs are built DIRECTLY from work/struct_layout.tsv (written by tools/structs_to_c.py) with explicit offsets - NOT through CParser,
// whose natural alignment shifted the tail of large structs. Enums still come from src/include/sdw_enums.h via CParser.
// For every struct S, every function named "S_*" that the decompiler sees as __thiscall (or __fastcall with one parameter = this-only)
// gets __thiscall committed and is moved into class namespace S, so the decompiler prints this->field.
// Headless: analyzeHeadless <proj> SDW -process SheepD3D.exe -noanalysis -scriptPath tools/ghidra \
//           -postScript ApplyClassTypes.java <repo>/src/include <repo>/work/struct_layout.tsv
// Run AFTER ApplySymbols.java and BEFORE ExportDecomp.java.
//@category SDW
import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParser;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolTable;

public class ApplyClassTypes extends GhidraScript {
    private DataTypeManager dtm;
    private final Map<String, Structure> built = new LinkedHashMap<>();

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        dtm = currentProgram.getDataTypeManager();

        // ---- enums via CParser (no layout issues there)
        StringBuilder src = new StringBuilder();
        for (String line : Files.readAllLines(Paths.get(args[0], "sdw_enums.h"), StandardCharsets.UTF_8)) {
            int c = line.indexOf("//");
            String l = c >= 0 ? line.substring(0, c) : line;
            if (!l.trim().startsWith("#")) {
                src.append(l).append('\n');
            }
        }
        try {
            new CParser(dtm, true, null).parse(new ByteArrayInputStream(src.toString().getBytes(StandardCharsets.UTF_8)));
        } catch (Exception e) {
            println("ApplyClassTypes: enum parse problem: " + e.getMessage());
        }

        // ---- structs from the layout table
        List<String[]> rows = new ArrayList<>();
        Map<String, Integer> sizes = new LinkedHashMap<>();
        for (String line : Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8)) {
            if (line.startsWith("#") || line.isBlank()) {
                continue;
            }
            String[] c = line.split("\t", 7);
            rows.add(c);
            int end = Integer.decode(c[2]) + Integer.decode(c[3]);
            int declared = Integer.decode(c[1]);
            sizes.merge(c[0], Math.max(declared, end), Math::max);
        }
        purge("Vec3s");
        purge("Box");
        StructureDataType vec = new StructureDataType("Vec3s", 0);
        vec.add(ShortDataType.dataType, "x", null);
        vec.add(ShortDataType.dataType, "y", "vertical axis, points DOWN");
        vec.add(ShortDataType.dataType, "z", null);
        built.put("Vec3s", (Structure) dtm.addDataType(vec, DataTypeConflictHandler.REPLACE_HANDLER));
        StructureDataType box = new StructureDataType("Box", 0);
        box.add(UnsignedIntegerDataType.dataType, "flags", null);
        box.add(new ArrayDataType(ShortDataType.dataType, 3, 2), "min", null);
        box.add(new ArrayDataType(ShortDataType.dataType, 3, 2), "max", null);
        built.put("Box", (Structure) dtm.addDataType(box, DataTypeConflictHandler.REPLACE_HANDLER));

        for (Map.Entry<String, Integer> e : sizes.entrySet()) {   // pass 1: empty shells so pointers can refer to any struct
            purge(e.getKey());
            StructureDataType s = new StructureDataType(e.getKey(), e.getValue());
            built.put(e.getKey(), (Structure) dtm.addDataType(s, DataTypeConflictHandler.REPLACE_HANDLER));
        }
        int placed = 0, failed = 0;
        for (String[] c : rows) {                                  // pass 2: fields at their exact offsets
            Structure s = built.get(c[0]);
            int off = Integer.decode(c[2]);
            int len = Integer.decode(c[3]);
            try {
                DataType ft = resolve(c[4], len);
                s.replaceAtOffset(off, ft, ft.getLength() > 0 ? ft.getLength() : len, c[5], c.length > 6 ? c[6] : null);
                placed++;
            } catch (Exception ex) {
                failed++;
                println("ApplyClassTypes: " + c[0] + "+" + c[2] + " " + c[5] + " (" + c[4] + "): " + ex.getMessage());
            }
        }

        // ---- class namespaces + thiscall
        SymbolTable st = currentProgram.getSymbolTable();
        Namespace global = currentProgram.getGlobalNamespace();
        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);
        int moved = 0, classes = 0, seen = 0;
        for (String s : sizes.keySet()) {
            GhidraClass cls = null;
            Namespace existing = st.getNamespace(s, global);
            if (existing instanceof GhidraClass) {
                cls = (GhidraClass) existing;
            }
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                if (!f.getName().startsWith(s + "_")) {
                    continue;
                }
                String cc = f.getCallingConventionName();
                boolean thisOnly = false;
                if (!"__thiscall".equals(cc)) {
                    DecompileResults r = ifc.decompileFunction(f, 30, monitor);
                    HighFunction hf = r == null ? null : r.getHighFunction();
                    if (hf == null) {
                        continue;
                    }
                    cc = hf.getFunctionPrototype().getModelName();
                    thisOnly = "__fastcall".equals(cc) && hf.getFunctionPrototype().getNumParams() == 1;
                }
                seen++;
                if (!"__thiscall".equals(cc) && !thisOnly) {
                    continue;
                }
                if (cls == null) {
                    cls = st.createClass(global, s, SourceType.USER_DEFINED);
                    classes++;
                }
                if (f.getParentNamespace() != cls) {
                    try {
                        f.setCallingConvention("__thiscall");
                        f.setParentNamespace(cls);
                        moved++;
                    } catch (Exception e) {
                        println("ApplyClassTypes: could not move " + f.getName() + ": " + e.getMessage());
                    }
                }
            }
        }
        ifc.dispose();
        println("ApplyClassTypes: " + sizes.size() + " structs built (" + placed + " fields placed, " + failed + " failed); " + seen +
            " candidate methods, " + classes + " new class namespaces, " + moved + " methods newly typed");
    }

    /** Remove every existing data type with this name (including ".conflict" leftovers from earlier CParser imports). */
    private void purge(String name) {
        List<DataType> found = new ArrayList<>();
        dtm.findDataTypes(name, found);
        dtm.findDataTypes(name + ".conflict", found);
        for (DataType d : found) {
            if (d instanceof Structure || d instanceof TypeDef) {
                dtm.remove(d, monitor);
            }
        }
    }

    private DataType resolve(String ctype, int len) {
        String t = ctype.trim();
        Matcher m = Pattern.compile("(.+?)\\s*\\[(\\d+|0x[0-9a-fA-F]+)\\]").matcher(t);
        if (m.matches()) {
            DataType base = resolve(m.group(1), 0);
            int n = Integer.decode(m.group(2));
            return new ArrayDataType(base, n, base.getLength());
        }
        if (t.endsWith("*")) {
            String base = t.substring(0, t.length() - 1).trim();
            Structure target = built.get(base);
            return new PointerDataType(target != null ? target : VoidDataType.dataType, 4);
        }
        switch (t) {
            case "s8": case "char": return SignedByteDataType.dataType;
            case "u8": case "bool8": return ByteDataType.dataType;
            case "s16": return ShortDataType.dataType;
            case "u16": return UnsignedShortDataType.dataType;
            case "s32": case "int": return IntegerDataType.dataType;
            case "u32": return UnsignedIntegerDataType.dataType;
            case "float": return FloatDataType.dataType;
            case "fnptr": return new PointerDataType(VoidDataType.dataType, 4);
            default:
                Structure s = built.get(t);
                return s != null ? s : UnsignedIntegerDataType.dataType;
        }
    }
}
