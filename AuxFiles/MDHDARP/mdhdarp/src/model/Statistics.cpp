#include "Statistics.h"

void Statistics::printStatistics() {
    // printf("\n*****STATISTICS******\n");
    // printf("\nTotal runtime: %.4lf\n", allProgram.printTime());
    printf("%.4lf\n", allProgram.printTime());
    // printf(" %.4lf, %.4lf,", constructive.printTime(), rt_construc.printTime());
    // printf("LS time = %.4lf\n", localSearch.printTime());
    // printf(" %.4lf, %.4lf, %.4lf, %.4lf, %.4lf, %.4lf, %.4lf, %.4lf,", shift.printTime(), rt_shift.printTime(), 
    // swap.printTime(), rt_swap.printTime(), cross.printTime(), rt_cross.printTime(), swapvhc.printTime(), rt_swapvhc.printTime());

    // printf(" %.4lf, %.4lf, %.4lf, %.4lf,", reinsertion.printTime(), rt_reins.printTime(), 
    // exchange2.printTime(), rt_exchange.printTime());
    // printf(" %.4lf, %.4lf, %.4lf, %.4lf,", shiftblock.printTime(), rt_shiftblock.printTime(), 
    // swapblock.printTime(), rt_swapblock.printTime());
    // // printf(" %.4lf,", perturbation.printTime());
    // printf(" %.4lf\n", ads_update.printTime());

    // printf("Load Data %.4lf\nILS: %.1lf -> %.4lf\nLocal Search: : %.1lf -> %.4lf\n", 
    // loadData.printTime(), ILS.count, ILS.printTime(), 
    // localSearch.count, localSearch.printTime());

    // printf("Constructive: %.1lf -> %.4lf\n", constructive.count, constructive.printTime());

    // printf("Shift: %.1lf -> %.4lf\nSwap: %.1lf -> %.4lf\nCross: %.1lf -> %.4lf\nSwap Vhc: %.1lf -> %.4lf\nSwap Block: %.1lf -> %.4lf\nShift Block: %.1lf -> %.4lf\n",
    // shift.count, shift.printTime(), swap.count, swap.printTime(), cross.count, cross.printTime(), swapvhc.count, swapvhc.printTime(), 
    // swapblock.count,  swapblock.printTime(), shiftblock.count, shiftblock.printTime());

    // printf("Reinsertion: %.1lf -> %.4lf\nExchange: %.1lf -> %.4lf\nExchangeReq: %.1lf -> %.4lf\n",
    // reinsertion.count, reinsertion.printTime(), exchange.count, exchange.printTime(), exchange2.count, exchange2.printTime());

    // printf("Perturbation: %.1lf -> %.4lf\nSet Partition: %.1lf -> %.4lf\nSet Partition In: %.1lf -> %.4lf\nRide Time: %.1lf -> %.4lf\n",
    // perturbation.count, perturbation.printTime(), SP.count, SP.printTime(), SPIn.count, SPIn.printTime(), rt.count, rt.printTime());

    // printf("Update Hash: %.1lf -> %.4lf\n", hashUpdate.count, hashUpdate.printTime());

    // printf("SetSeq: %.1lf -> %.4lf\nConcat Seq: %.1lf -> %.4lf\n",
    // setSeq.count, setSeq.printTime(), concatSeq.count, concatSeq.printTime());
}

void Statistics::printImproves() {
    printf("Swp: %d - %.2lf | Shft: %d - %.2lf | Cross: %d - %.2lf | SwpV: %d - %.2lf | SwapB: %d - %.2lf | ShiftB: %d - %.2lf | Exc: %d - %.2lf | Rsrt: %d - %.2lf | ExReq: %d - %.2lf\n",
           improves[0], (improvesPercent[0]/improves[0] * 100), improves[1], (improvesPercent[1]/improves[1] * 100), 
           improves[2], (improvesPercent[2]/improves[2] * 100), improves[3], (improvesPercent[3]/improves[3] * 100),
           improves[4], (improvesPercent[4]/improves[4] * 100), improves[5], (improvesPercent[5]/improves[5] * 100),
           improves[6], (improvesPercent[6]/improves[6] * 100), improves[7], (improvesPercent[7]/improves[7] * 100),
           improves[8], (improvesPercent[8]/improves[8] * 100));
}