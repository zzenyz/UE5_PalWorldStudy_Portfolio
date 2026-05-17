#include "PalStudyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PalStudySaveGame.h"
#include "PalWorld_StudyCharacter.h"
#include "PalStorageComponent.h"
#include "InventoryComponent.h"

UPalStudyGameInstance::UPalStudyGameInstance()
{
    SaveSlotName = TEXT("PalStudy_SaveSlot_1");
}

void UPalStudyGameInstance::SavePlayerData()
{
    UPalStudySaveGame* SaveGameObj = Cast<UPalStudySaveGame>(UGameplayStatics::CreateSaveGameObject(UPalStudySaveGame::StaticClass()));
    if (!SaveGameObj) return;

    // 플레이어 데이터 저장
    APalWorld_StudyCharacter* PlayerCharacter = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter)
    {
        if (auto* Storage = PlayerCharacter->FindComponentByClass<UPalStorageComponent>())
            SaveGameObj->SavedPals = Storage->StoredPalsData;

        if (auto* Inv = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
        {
            for (const FItemSlot& Slot : Inv->Slots)
            {
                FSavedItemSlot S;
                S.Quantity = Slot.Quantity;
                if (Slot.ItemAsset) S.ItemID = Slot.ItemAsset->ItemID;
                SaveGameObj->SavedInventory.Add(S);
            }
        }
    }

    // 스포너 데이터 저장
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Spawner"), FoundSpawners);

    if (FoundSpawners.Num() > 0)
    {
        AActor* Spawner = FoundSpawners[0];

        if (auto* SInv = Spawner->FindComponentByClass<UInventoryComponent>())
        {
            for (const FItemSlot& Slot : SInv->Slots)
            {
                FSavedItemSlot S;
                S.Quantity = Slot.Quantity;
                if (Slot.ItemAsset) S.ItemID = Slot.ItemAsset->ItemID;
                SaveGameObj->SpawnerInventory.Add(S);
            }
        }

        TArray<UPalStorageComponent*> Comps;
        Spawner->GetComponents<UPalStorageComponent>(Comps);
        for (auto* Comp : Comps)
        {
            if (Comp->GetName().Contains(TEXT("Active")))
                SaveGameObj->SpawnerActivePals = Comp->StoredPalsData;
            else
                SaveGameObj->SpawnerPals = Comp->StoredPalsData;
        }
    }

    UGameplayStatics::SaveGameToSlot(SaveGameObj, SaveSlotName, 0);
}

void UPalStudyGameInstance::LoadPlayerData()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) return;
    UPalStudySaveGame* LoadGameObj = Cast<UPalStudySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (!LoadGameObj) return;

    // 플레이어 데이터 복구
    APalWorld_StudyCharacter* PlayerCharacter = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter)
    {
        if (auto* Storage = PlayerCharacter->FindComponentByClass<UPalStorageComponent>())
        {
            Storage->StoredPalsData = LoadGameObj->SavedPals;
            Storage->OnPalStorageUpdated.Broadcast();
        }

        UInventoryComponent* InvComp = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
        if (InvComp && ItemDataTable)
        {
            InvComp->Slots.Empty();
            InvComp->Slots.SetNum(InvComp->GetCapacity());
            for (int32 i = 0; i < LoadGameObj->SavedInventory.Num(); ++i)
            {
                if (!InvComp->Slots.IsValidIndex(i)) break;
                const FSavedItemSlot& S = LoadGameObj->SavedInventory[i];
                InvComp->Slots[i].Quantity = S.Quantity;
                if (!S.ItemID.IsNone())
                {
                    FItemDataRow* Row = ItemDataTable->FindRow<FItemDataRow>(S.ItemID, TEXT(""));
                    if (Row) InvComp->Slots[i].ItemAsset = Row->ItemAsset;
                }
            }
            InvComp->OnInventoryUpdated.Broadcast();
        }
    }

    // 스포너 데이터 복구
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Spawner"), FoundSpawners);

    if (FoundSpawners.Num() > 0)
    {
        AActor* Spawner = FoundSpawners[0];

        UInventoryComponent* SInv = Spawner->FindComponentByClass<UInventoryComponent>();
        if (SInv && ItemDataTable)
        {
            SInv->Slots.Empty();
            SInv->Slots.SetNum(SInv->GetCapacity());
            for (int32 i = 0; i < LoadGameObj->SpawnerInventory.Num(); ++i)
            {
                if (!SInv->Slots.IsValidIndex(i)) break;
                const FSavedItemSlot& S = LoadGameObj->SpawnerInventory[i];
                SInv->Slots[i].Quantity = S.Quantity;
                if (!S.ItemID.IsNone())
                {
                    FItemDataRow* Row = ItemDataTable->FindRow<FItemDataRow>(S.ItemID, TEXT(""));
                    if (Row) SInv->Slots[i].ItemAsset = Row->ItemAsset;
                }
            }
            SInv->OnInventoryUpdated.Broadcast();
        }

        TArray<UPalStorageComponent*> Comps;
        Spawner->GetComponents<UPalStorageComponent>(Comps);
        for (auto* Comp : Comps)
        {
            if (Comp->GetName().Contains(TEXT("Active")))
                Comp->StoredPalsData = LoadGameObj->SpawnerActivePals;
            else
                Comp->StoredPalsData = LoadGameObj->SpawnerPals;

            Comp->OnPalStorageUpdated.Broadcast();
        }
    }
}