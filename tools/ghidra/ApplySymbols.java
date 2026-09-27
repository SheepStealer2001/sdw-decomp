// Apply the git-tracked symbol database to the Ghidra program. The CSVs are the source of truth for names;
// the Ghidra project is disposable and can be rebuilt from the exe + these files.
//   columns: address,name,kind,comment      kind = func | data | vtable | label
// Headless: analyzeHeadless <proj> SDW -process SheepD3D.exe -noanalysis -scriptPath tools/ghidra \
//           -postScript ApplySymbols.java data/symbols_auto.csv data/symbols.csv
// Later files win, so hand-curated names (symbols.csv) override generated ones (symbols_auto.csv).
//@category SDW
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplySymbols extends GhidraScript {
    @Override
    public void run() throws Exception {
        int funcs = 0, labels = 0, skipped = 0;
        for (String file : getScriptArgs()) {
            List<String> lines = Files.readAllLines(Paths.get(file), StandardCharsets.UTF_8);
            for (String line : lines.subList(1, lines.size())) {
                if (line.isBlank() || line.startsWith("#")) {
                    continue;
                }
                String[] c = line.split(",", 4);
                if (c.length < 3) {
                    skipped++;
                    continue;
                }
                Address a = toAddr(Long.decode(c[0].trim()));
                String name = c[1].trim();
                String kind = c[2].trim();
                String comment = c.length > 3 ? c[3].trim().replaceAll("^\"|\"$", "").replace("\"\"", "\"") : "";

                if (kind.equals("func")) {
                    Function f = getFunctionAt(a);
                    if (f == null) {
                        f = createFunction(a, name);
                    }
                    if (f == null) {
                        println("ApplySymbols: no function at " + a + " for " + name);
                        skipped++;
                        continue;
                    }
                    f.setName(name, SourceType.USER_DEFINED);
                    if (!comment.isEmpty()) {
                        f.setComment(comment);
                    }
                    funcs++;
                } else {
                    createLabel(a, name, true, SourceType.USER_DEFINED);
                    if (!comment.isEmpty()) {
                        setPlateComment(a, comment);
                    }
                    labels++;
                }
            }
        }
        println("ApplySymbols: " + funcs + " functions, " + labels + " labels, " + skipped + " skipped");
    }
}
