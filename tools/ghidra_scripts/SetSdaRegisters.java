// Pre-analysis: set r13/r2 to _SDA_BASE_/_SDA2_BASE_ over all executable memory, so
// small-data loads and stores resolve to globals.
// @category LSW1

import ghidra.app.script.GhidraScript;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.ProgramContext;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Symbol;

import java.math.BigInteger;

public class SetSdaRegisters extends GhidraScript {
    private void apply(String symbolName, String registerName) throws Exception {
        Symbol symbol = getSymbol(symbolName, null);
        if (symbol == null) {
            println("warning: " + symbolName + " missing; " + registerName + " left unset");
            return;
        }
        Register register = currentProgram.getRegister(registerName);
        ProgramContext context = currentProgram.getProgramContext();
        BigInteger value = BigInteger.valueOf(symbol.getAddress().getOffset());
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            if (block.isExecute()) {
                context.setValue(register, block.getStart(), block.getEnd(), value);
            }
        }
        println(registerName + " = " + symbol.getAddress() + " (" + symbolName + ")");
    }

    @Override
    protected void run() throws Exception {
        apply("_SDA_BASE_", "r13");
        apply("_SDA2_BASE_", "r2");
    }
}
