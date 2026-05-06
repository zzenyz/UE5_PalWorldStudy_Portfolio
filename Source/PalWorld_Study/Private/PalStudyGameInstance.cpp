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

    // --- [1] 플레이어 데이터 저장 ---
    APalWorld_StudyCharacter* PlayerCharacter = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter)
    {
        if (auto* Storage = PlayerCharacter->FindComponentByClass<UPalStorageComponent>())
            SaveGameObj->SavedPals = Storage->StoredPalsData; // 팰 저장

        if (auto* Inv = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
        {
            for (const FItemSlot& Slot : Inv->Slots)
            {
                FSavedItemSlot S; S.Quantity = Slot.Quantity;
                if (Slot.ItemAsset) S.ItemID = Slot.ItemAsset->ItemID;
                SaveGameObj->SavedInventory.Add(S); // 아이템 저장
            }
        }
    }

    // --- [2] BP 스포너 데이터 저장 (태그 활용) ---
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Spawner"), FoundSpawners); // ★ 태그로 검색

    if (FoundSpawners.Num() > 0)
    {
        AActor* Spawner = FoundSpawners[0]; // 첫 번째 스포너 사용

        // 스포너 인벤토리 저장
        if (auto* SInv = Spawner->FindComponentByClass<UInventoryComponent>())
        {
            for (const FItemSlot& Slot : SInv->Slots)
            {
                FSavedItemSlot S; S.Quantity = Slot.Quantity;
                if (Slot.ItemAsset) S.ItemID = Slot.ItemAsset->ItemID;
                SaveGameObj->SpawnerInventory.Add(S);
            }
        }

        // 스포너 팰 (보관함 + 작업장) 저장
        TArray<UPalStorageComponent*> Comps;
        Spawner->GetComponents<UPalStorageComponent>(Comps);
        for (auto* Comp : Comps)
        {
            // BP에서 컴포넌트 이름을 "Active" 등으로 지었다면 이름으로 구분 가능합니다.
            if (Comp->GetName().Contains(TEXT("Active")))
                SaveGameObj->SpawnerActivePals = Comp->StoredPalsData;
            else
                SaveGameObj->SpawnerPals = Comp->StoredPalsData;
        }
    }

    UGameplayStatics::SaveGameToSlot(SaveGameObj, SaveSlotName, 0);
    UE_LOG(LogTemp, Warning, TEXT("player and spawner save success"));
}

void UPalStudyGameInstance::LoadPlayerData()
{
    // 1. 세이브 파일 존재 확인 및 로드
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) return;
    UPalStudySaveGame* LoadGameObj = Cast<UPalStudySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (!LoadGameObj) return;

    // --- [1] 플레이어 데이터 복구 ---
    APalWorld_StudyCharacter* PlayerCharacter = Cast<APalWorld_StudyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (PlayerCharacter)
    {
        // 플레이어 팰 복구
        if (auto* Storage = PlayerCharacter->FindComponentByClass<UPalStorageComponent>())
        {
            Storage->StoredPalsData = LoadGameObj->SavedPals;
            Storage->OnPalStorageUpdated.Broadcast();
        }

        // 플레이어 인벤토리 복구 (ID 기반)
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

    // --- [2] BP 스포너 데이터 복구 (가연 님이 말씀하신 부분!) ---
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Spawner"), FoundSpawners);

    if (FoundSpawners.Num() > 0)
    {
        AActor* Spawner = FoundSpawners[0];

        // 스포너 인벤토리 복구
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

        // 스포너 팰 복구 (Active 구분)
        TArray<UPalStorageComponent*> Comps;
        Spawner->GetComponents<UPalStorageComponent>(Comps);
        for (auto* Comp : Comps)
        {
            // 컴포넌트 이름에 "Active"가 포함되어 있으면 작업 중인 팰 데이터를 넣습니다.
            if (Comp->GetName().Contains(TEXT("Active")))
                Comp->StoredPalsData = LoadGameObj->SpawnerActivePals;
            else
                Comp->StoredPalsData = LoadGameObj->SpawnerPals;

            Comp->OnPalStorageUpdated.Broadcast();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("player and spawner load success"));
}