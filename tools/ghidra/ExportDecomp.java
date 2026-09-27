// Decompile every function below the MSVC CRT to work/decomp/<addr>_<name>.c and write an index
// (work/ghidra_functions.json) with sizes, callers and callees. The static CRT (0x565850+, anchored by
// FID matches for operator new/strlen/memset) is indexed but not decompiled. libjpeg and DirectX SDK
// utility code live below that line, interleaved with game code, and are tagged separately once identified.
// Headless: analyzeHeadless <proj> SDW -process SheepD3D.exe -noanalysis -scriptPath tools/ghidra \
//           -postScript ExportDecomp.java <outDir>
// The export is ATOMIC: files are written to <outDir>/decomp.tmp and swapped into place at the end, so a search of
// work/decomp/ while a re-export runs never sees a half-written or missing corpus. Do not `rm -rf work/decomp` first.
//@category SDW
import java.io.File;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.Set;
import java.util.TreeSet;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

public class ExportDecomp extends GhidraScript {
    private static final long GAME_LO = 0x401000L;
    private static final long GAME_HI = 0x565850L;

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = new File(args.length > 0 ? args[0] : "work");
        File finalDir = new File(outDir, "decomp");
        File decompDir = new File(outDir, "decomp.tmp");
        deleteTree(decompDir);
        decompDir.mkdirs();
        File finalIdx = new File(outDir, "ghidra_functions.json");
        File tmpIdx = new File(outDir, "ghidra_functions.json.tmp");

        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);

        int total = 0, done = 0, failed = 0;
        try (PrintWriter idx = new PrintWriter(tmpIdx, StandardCharsets.UTF_8)) {
            idx.println("{");
            FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
            boolean first = true;
            while (it.hasNext() && !monitor.isCancelled()) {
                Function f = it.next();
                if (f.isExternal() || f.isThunk()) {
                    continue;
                }
                long addr = f.getEntryPoint().getOffset();
                boolean game = addr >= GAME_LO && addr < GAME_HI;
                total++;

                Set<String> callers = new TreeSet<>();
                for (Function c : f.getCallingFunctions(monitor)) {
                    callers.add(String.format("%08x", c.getEntryPoint().getOffset()));
                }
                Set<String> callees = new TreeSet<>();
                for (Function c : f.getCalledFunctions(monitor)) {
                    callees.add(c.isExternal() ? c.getName() : String.format("%08x", c.getEntryPoint().getOffset()));
                }

                if (!first) {
                    idx.println(",");
                }
                first = false;
                idx.printf(" \"%08x\": {\"name\": \"%s\", \"size\": %d, \"game\": %b, \"callers\": %s, \"callees\": %s}",
                    addr, f.getName(), f.getBody().getNumAddresses(), game, json(callers), json(callees));

                if (!game) {
                    continue;
                }
                DecompileResults r = ifc.decompileFunction(f, 60, monitor);
                if (r != null && r.decompileCompleted()) {
                    File out = new File(decompDir, String.format("%08x_%s.c", addr, f.getName().replaceAll("[^A-Za-z0-9_]", "_")));
                    try (PrintWriter w = new PrintWriter(out, StandardCharsets.UTF_8)) {
                        w.printf("// %08x %s  size=%d callers=%d%n", addr, f.getName(), f.getBody().getNumAddresses(), callers.size());
                        w.print(r.getDecompiledFunction().getC());
                    }
                    done++;
                } else {
                    failed++;
                }
            }
            idx.println();
            idx.println("}");
        }
        ifc.dispose();
        // swap into place: two renames, a window of microseconds instead of a minute
        File old = new File(outDir, "decomp.old");
        deleteTree(old);
        if (finalDir.exists()) {
            Files.move(finalDir.toPath(), old.toPath(), StandardCopyOption.ATOMIC_MOVE);
        }
        Files.move(decompDir.toPath(), finalDir.toPath(), StandardCopyOption.ATOMIC_MOVE);
        Files.move(tmpIdx.toPath(), finalIdx.toPath(), StandardCopyOption.REPLACE_EXISTING, StandardCopyOption.ATOMIC_MOVE);
        deleteTree(old);
        println("ExportDecomp: " + total + " functions indexed, " + done + " decompiled, " + failed + " failed");
    }

    private static void deleteTree(File f) {
        if (!f.exists()) {
            return;
        }
        File[] kids = f.listFiles();
        if (kids != null) {
            for (File k : kids) {
                deleteTree(k);
            }
        }
        f.delete();
    }

    private static String json(Set<String> s) {
        StringBuilder b = new StringBuilder("[");
        for (String x : s) {
            if (b.length() > 1) {
                b.append(", ");
            }
            b.append('"').append(x.replace("\\", "\\\\").replace("\"", "\\\"")).append('"');
        }
        return b.append(']').toString();
    }
}
