// Create functions at entry points that auto-analysis cannot discover because they are only ever referenced
// as immediates (factories/callbacks registered at runtime). Input: text file, one hex VA per line.
// A candidate is accepted only if it starts with the MSVC frame prologue 55 8B EC (all game code is
// EBP-framed), which rejects data constants that merely fall inside the .text range.
// Headless (run WITH analysis so newly found code is followed):
//   analyzeHeadless <proj> SDW -process SheepD3D.exe -scriptPath tools/ghidra -preScript SeedFunctions.java <file>
//@category SDW
import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

public class SeedFunctions extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        int created = 0, existing = 0, rejected = 0;
        for (String line : Files.readAllLines(Paths.get(args[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }
            Address a = toAddr(Long.parseLong(line.replaceFirst("^0x", ""), 16));
            if (getFunctionAt(a) != null) {
                existing++;
                continue;
            }
            byte[] b = getBytes(a, 3);
            if ((b[0] & 0xff) != 0x55 || (b[1] & 0xff) != 0x8b || (b[2] & 0xff) != 0xec) {
                println("SeedFunctions: rejected " + a + " (no frame prologue)");
                rejected++;
                continue;
            }
            if (getInstructionAt(a) == null) {
                // Clear whatever a misaligned sweep left here, then disassemble from the true entry.
                clearListing(a, a.add(2));
                new DisassembleCommand(a, null, true).applyTo(currentProgram, monitor);
            }
            Function f = createFunction(a, null);
            if (f != null) {
                created++;
            } else {
                println("SeedFunctions: could not create function at " + a);
            }
        }
        println("SeedFunctions: created " + created + ", already present " + existing + ", rejected " + rejected);
    }
}
