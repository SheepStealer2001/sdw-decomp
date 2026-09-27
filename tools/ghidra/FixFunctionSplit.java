// FixFunctionSplit.java - undo a bogus function split. Ghidra (or an earlier ApplySymbols run on a wrong "func" row) sometimes creates a
// function in the middle of another one, cutting the real function's body short. For each pair CHUNK:OWNER this removes the function at
// CHUNK and re-computes OWNER's body so it covers the chunk again. ApplySymbols never deletes functions, so this is the explicit fix.
//   -postScript FixFunctionSplit.java 0x4ed99f:0x4ed66d [more pairs...]
// Only use it for a CHUNK proven to be a fall-through (no prologue, uses the caller's frame) - see data/symbols.csv for the evidence.
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

public class FixFunctionSplit extends GhidraScript {
    @Override
    public void run() throws Exception {
        for (String arg : getScriptArgs()) {
            String[] p = arg.split(":");
            Address chunk = toAddr(Long.decode(p[0]));
            Address owner = toAddr(Long.decode(p[1]));
            Function bogus = getFunctionAt(chunk);
            Function real = getFunctionAt(owner);
            if (real == null) {
                println("FixFunctionSplit: no function at owner " + owner + ", skipped");
                continue;
            }
            long before = real.getBody().getNumAddresses();
            if (bogus != null) {
                removeFunction(bogus);
            }
            CreateFunctionCmd.fixupFunctionBody(currentProgram, real, monitor);
            println("FixFunctionSplit: " + (bogus != null ? "removed " + chunk + ", " : "no function at " + chunk + ", ") + real.getName()
                    + " body " + before + " -> " + real.getBody().getNumAddresses() + " bytes");
        }
    }
}
