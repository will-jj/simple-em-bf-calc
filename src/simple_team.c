#include <stddef.h>
#include <stdio.h>
#include "types.h"
#include "battle_frontier_mons.h"
#include "battle_frontier.h"
#include "species.h"
#include "items.h"
#include "battle_frontier_mons_s.h"

#define FRONTIER_PARTY_SIZE 3
#define MAX_FRONTIER_PARTY_SIZE FRONTIER_PARTY_SIZE
#define FRONTIER_LVL_50 0
#define FRONTIER_LVL_OPEN 1
#define MAX_MON_MOVES 4

u32 gRngValue = 0;
struct FacilityMon *gFacilityTrainerMons = gBattleFrontierMons;
u16 gFrontierTempParty[MAX_FRONTIER_PARTY_SIZE] = {0};
unsigned long teamPlayas[NUM_FRONTIER_MONS][FRONTIER_PARTY_SIZE] = {0};

u16 Random(void)
{
    // ISO_RANDOMIZE1
    gRngValue = (1103515245 * (gRngValue) + 24691);
    return gRngValue >> 16;
}

u16 GetFactoryMonId(u8 lvlMode, u8 challengeNum)
{
    u16 numMons, monId;
    u16 adder; // Used to skip past early mons for open level

    if (lvlMode == FRONTIER_LVL_50)
        adder = 0;
    else
        adder = 8;

    u16 challenge = challengeNum;
    if (challenge != 7)
        challenge = 7; // why bother assigning it above at all

    numMons = (sInitialRentalMonRanges[adder + challenge][1] - sInitialRentalMonRanges[adder + challenge][0]) + 1;
    monId = Random() % numMons;
    monId += sInitialRentalMonRanges[adder + challenge][0];

    return monId;
}

void FillFactoryBrainParty(void)
{
    int i, j, k;
    u16 species[FRONTIER_PARTY_SIZE];
    u16 heldItems[FRONTIER_PARTY_SIZE];
    u16 monIDs[FRONTIER_PARTY_SIZE];
    u8 friendship;
    int monLevel;
    u8 fixedIV;
    u32 otId;

    u8 lvlMode = FRONTIER_LVL_OPEN;
    u8 challengeNum = 7;
    fixedIV = 31;
    monLevel = 100;
    i = 0;
    otId = 1;

    while (i != FRONTIER_PARTY_SIZE)
    {
        u16 monId = GetFactoryMonId(lvlMode, challengeNum);

        if (gFacilityTrainerMons[monId].species == SPECIES_UNOWN)
            continue;
        if (monLevel == FRONTIER_MAX_LEVEL_50 && monId > FRONTIER_MONS_HIGH_TIER)
            continue;

        species[i] = gFacilityTrainerMons[monId].species;
        heldItems[i] = gBattleFrontierHeldItems[gFacilityTrainerMons[monId].itemTableId];

        // Skip rental mon check (i.e, removed chunk of code)

        // only check second and third mon
        if (i != 0)
        {
            for (k = 0; k < i; k++)
            {
                if (species[k] == species[i])
                    break;
            }
            if (k != i)
                continue;

            for (k = 0; k < i; k++)
            {
                if (heldItems[k] != ITEM_NONE && heldItems[k] == heldItems[i])
                    break;
            }
            if (k != i)
                continue;
        }


        // increment the mon id in slot count
        teamPlayas[monId][i]++;
        i++;
    }
}

void WriteTeamsToCSV(void)
{
    FILE *fp = fopen("teamPlayasMax.csv", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(fp, "Mon, Slot-1, Slot-2, Slot-3\n");

    for (int m = 0; m < NUM_FRONTIER_MONS; m++)
    {
        fprintf(fp, "%s, %lu, %lu, %lu\n",
                gBattleFrontierMons[m].name, teamPlayas[m][0], teamPlayas[m][1], teamPlayas[m][2]);
    }

    fclose(fp);
}

void main(void)
{

    for (unsigned long ii = 0; ii <= UINT32_MAX; ii++)
    {
        if (ii % 1000000 == 0)
        {
            printf("Team %lu\n", ii);
        }
        FillFactoryBrainParty();
    }

    WriteTeamsToCSV();

    return;
}