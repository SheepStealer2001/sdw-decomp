// Parse src/include/scenaric_props.h into the program's data type manager under /SDW/ScenaricProps.
// Headless: analyzeHeadless <proj> SDW -process SheepD3D.exe -noanalysis -scriptPath tools/ghidra \
//           -postScript ImportScenaricTypes.java <path/to/scenaric_props.h>
//@category SDW
import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParser;
import ghidra.program.model.data.CategoryPath;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeManager;

public class ImportScenaricTypes extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("usage: ImportScenaricTypes.java <scenaric_props.h>");
        }
        // CParser has no preprocessor: drop directives and // comments.
        StringBuilder src = new StringBuilder();
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.ISO_8859_1)) {
            if (line.startsWith("#")) {
                continue;
            }
            int c = line.indexOf("//");
            src.append(c >= 0 ? line.substring(0, c) : line).append('\n');
        }

        DataTypeManager dtm = currentProgram.getDataTypeManager();
        CParser parser = new CParser(dtm, true, null);
        parser.parse(new ByteArrayInputStream(src.toString().getBytes(StandardCharsets.ISO_8859_1)));

        CategoryPath dest = new CategoryPath("/SDW/ScenaricProps");
        List<DataType> moved = new ArrayList<>();
        dtm.getAllDataTypes(moved);
        int n = 0;
        for (DataType dt : moved) {
            String name = dt.getName();
            boolean ours = name.endsWith("Props") || name.equals("ScenaricClassId") || name.equals("u32");
            if (ours && dt.getCategoryPath().equals(CategoryPath.ROOT)) {
                dt.setCategoryPath(dest);
                n++;
            }
        }
        println("ImportScenaricTypes: " + n + " types placed in " + dest);
    }
}
