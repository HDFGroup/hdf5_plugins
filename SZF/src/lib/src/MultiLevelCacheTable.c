/**
 *  @file MultiLevelCacheTable.c
 *  @author Xiangyu Zou, Tao Lu, Wen Xia, Xuan Wang, Weizhe Zhang, Sheng Di, Dingwen Tao
 *  @date Jan, 2019
 *  @brief Header file.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "MultiLevelCacheTable.h"

uint8_t MLCT_GetExpoIndex(float value){
    uint32_t* ptr = (uint32_t*)&value;
    return (*ptr) >> 23;
}

uint8_t MLCT_GetRequiredBits(float precision){
    int32_t* ptr = (int32_t*)&precision;
    return -(((*ptr) >> 23) - 127);
}


uint32_t MLCT_GetMantiIndex(float value, int bits){
    uint32_t* ptr = (uint32_t*)&value;
    (*ptr) = (*ptr) << 9 >> 9;
    int shift = 32 - 9 - bits;
    if(shift > 0){
        return (*ptr) >> shift;
    }else{
        return (*ptr);
    }
}

float MLTC_RebuildFloat(uint8_t expo, uint32_t manti, int bits){
    float result = 0;
    uint32_t *ptr = (uint32_t*)&result;
    *ptr = expo;
    (*ptr) = (*ptr) << 23;
    (*ptr) |= (manti << (23-bits));
    return result;
}

void MultiLevelCacheTableBuild(struct TopLevelTable* topTable, float* precisionTable, int count, float precision){
    uint8_t bits = MLCT_GetRequiredBits(precision);
    topTable->bits = bits;
    topTable->bottomBoundary = precisionTable[1]/(1+precision);
    topTable->topBoundary = precisionTable[count-1]/(1-precision);
    topTable->baseIndex = MLCT_GetExpoIndex(topTable->bottomBoundary);
    topTable->topIndex = MLCT_GetExpoIndex(topTable->topBoundary);
    int subTableCount = topTable->topIndex - topTable->baseIndex + 1;
    topTable->subTables = (struct SubLevelTable*)malloc(sizeof(struct SubLevelTable) * subTableCount);
    memset(topTable->subTables, 0, sizeof(struct SubLevelTable) * subTableCount);

    //uint32_t expoBoundary[subTableCount];
    uint8_t lastExpo = 0xff;
    uint8_t lastIndex = 0;
    int i;
    for(i=0; i<count; i++){
        uint8_t expo = MLCT_GetExpoIndex(precisionTable[i]);
        if(expo != lastExpo){
            //expoBoundary[lastIndex] = i;
            lastExpo = expo;
            lastIndex++;
        }
    }

    for(i=topTable->topIndex-topTable->baseIndex; i>=0; i--){
        struct SubLevelTable* processingSubTable = &topTable->subTables[i];
        if(i == topTable->topIndex - topTable->baseIndex &&
            MLCT_GetExpoIndex(topTable->topBoundary) == MLCT_GetExpoIndex(precisionTable[count-1])){
            processingSubTable->topIndex = MLCT_GetMantiIndex(topTable->topBoundary, bits) - 1;
        }else{
            uint32_t maxIndex = 0;
            int j;
            for(j=0; j<bits; j++){
                maxIndex += 1 << j;
            }
            processingSubTable->topIndex = maxIndex;
        }
        if(i == 0 && MLCT_GetExpoIndex(topTable->bottomBoundary) == MLCT_GetExpoIndex(precisionTable[0])){
            processingSubTable->baseIndex = MLCT_GetMantiIndex(topTable->bottomBoundary, bits)+1;
        }else{
            processingSubTable->baseIndex = 0;
        }

        int subTableLength = processingSubTable->topIndex - processingSubTable-> baseIndex+ 1;
        processingSubTable->table = (uint32_t*)malloc(sizeof(uint32_t) * subTableLength);
        memset(processingSubTable->table, 0, sizeof(uint32_t) * subTableLength);
        processingSubTable->expoIndex = topTable->baseIndex + i;
    }

    uint32_t index = 1;
    uint8_t ui;
    uint32_t uj;
    for(ui = 0; ui<=topTable->topIndex-topTable->baseIndex; ui++){
        struct SubLevelTable* processingSubTable = &topTable->subTables[ui];
        uint8_t expoIndex = ui+topTable->baseIndex;
        for(uj = 0; uj<=processingSubTable->topIndex - processingSubTable->baseIndex; uj++){
            uint32_t mantiIndex = uj+processingSubTable->baseIndex;
            float sample = MLTC_RebuildFloat(expoIndex, mantiIndex, topTable->bits);
            float bottomBoundary = precisionTable[index] / (1+precision);
            float topBoundary = precisionTable[index] / (1-precision);
            if(sample < topBoundary && sample > bottomBoundary){
                processingSubTable->table[uj] = index;
            }else{
                //float newPrecision = precisionTable[index];
                index++;
                processingSubTable->table[uj] = index;
                if(uj)
                    processingSubTable->table[uj-1] = index;
                else{
                    struct SubLevelTable* pastSubTable = &topTable->subTables[ui-1];
                    pastSubTable->table[pastSubTable->topIndex - pastSubTable->baseIndex] = index;
                }
            }
        }
        if(ui == topTable->topIndex - topTable->baseIndex){
            uj = processingSubTable->topIndex - processingSubTable->baseIndex + 1;
            uint32_t mantiIndex = uj + processingSubTable->baseIndex;
            float sample = MLTC_RebuildFloat(expoIndex, mantiIndex, topTable->bits);
            float bottomBoundary = precisionTable[index] / (1+precision);
            float topBoundary = precisionTable[index] / (1-precision);
            if(sample > topBoundary || sample < bottomBoundary){
                index++;
                processingSubTable->table[uj-1] = index;
            }
        }
    }

    /*
    long lastIndexInExpoRange = count-1;
    bool trigger = false;
    float preRange = 0.0;
    uint32_t preIndex = 0;
    int ii;
    for(ii=topTable->topIndex-topTable->baseIndex; >=0; --){
        struct SubLevelTable* processingSubTable = &topTable->subTables[];
        if(trigger){
            uint32_t bound = MLCT_GetMantiIndex(preRange, bits);
            int jj;
            for(jj = processingSubTable->topIndex; jj>=processingSubTable->baseIndex; jj--){
                if(jj >= bound){
                    processingSubTable->table[jj-processingSubTable->baseIndex] = preIndex;
                }else{
                    break;
                }
            }
            trigger = false;
        }
        long firstIndexInExpoRange = expoBoundary[i];
        uint8_t expoInRange = MLCT_GetExpoIndex(precisionTable[firstIndexInExpoRange]);
        int jj;
        for(jj=lastIndexInExpoRange; jj>=firstIndexInExpoRange; jj--){
            float test = precisionTable[jj];
            uint32_t rangeTop = MLCT_GetMantiIndex(precisionTable[jj]*(1+precision), bits) - 1;
            uint32_t rangeBottom;
            if(jj == firstIndexInExpoRange){
                preRange = precisionTable[jj]/(1+precision);
                if(expoInRange != MLCT_GetExpoIndex(preRange)){
                    trigger = true;
                    preIndex = firstIndexInExpoRange;
                    rangeBottom = 0;
                }else{
                    rangeBottom= MLCT_GetMantiIndex(precisionTable[jj]/(1+precision), bits) + 1;
                }
            }else{
                rangeBottom= MLCT_GetMantiIndex(precisionTable[jj]/(1+precision), bits) + 1;
            }
            int kk;
            for(kk = rangeBottom; kk<=rangeTop; kk++){
                if( kk <= processingSubTable->topIndex && kk >= processingSubTable->baseIndex)
                    processingSubTable->table[kk - processingSubTable->baseIndex] = jj;
            }
        }
        lastIndexInExpoRange = firstIndexInExpoRange-1;
    }
     */
}

uint32_t MultiLevelCacheTableGetIndex(float value, struct TopLevelTable* topLevelTable){
    uint8_t expoIndex = MLCT_GetExpoIndex(value);
    if(expoIndex <= topLevelTable->topIndex && expoIndex >= topLevelTable->baseIndex){
        struct SubLevelTable* subLevelTable = &topLevelTable->subTables[expoIndex-topLevelTable->baseIndex];
        uint32_t mantiIndex = MLCT_GetMantiIndex(value, topLevelTable->bits);
        MLTC_RebuildFloat(expoIndex, mantiIndex, topLevelTable->bits);
        if(mantiIndex >= subLevelTable->baseIndex && mantiIndex <= subLevelTable->topIndex)
            return subLevelTable->table[mantiIndex - subLevelTable->baseIndex];
    }
    return 0;
}

void MultiLevelCacheTableFree(struct TopLevelTable* table){
    int i;
    for(i=0; i<table->topIndex - table->baseIndex + 1; i++){
        free(table->subTables[i].table);
    }
    free(table->subTables);
}
