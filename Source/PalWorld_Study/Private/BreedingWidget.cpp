#include "BreedingWidget.h"
#include "BreedingSlotWidget.h"
#include "PalSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Pal.h"
#include "BreedingFarm.h"

void UBreedingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	if (Btn_Breed)
	{
		Btn_Breed->OnClicked.RemoveAll(this);
		Btn_Breed->OnClicked.AddDynamic(this, &UBreedingWidget::OnBreedButtonClicked);
	}

	if (APawn* PlayerPawn = GetOwningPlayerPawn())
	{
		PlayerPalStorageComp = PlayerPawn->FindComponentByClass<UPalStorageComponent>();
		if (PlayerPalStorageComp.IsValid())
		{
			PlayerPalStorageComp->OnPalStorageUpdated.RemoveAll(this);
			PlayerPalStorageComp->OnPalStorageUpdated.AddDynamic(this, &UBreedingWidget::RefreshPalList);
		}
	}

	RefreshPalList();
}

void UBreedingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TargetFarm || !PB_EggProgress || !Text_BreedBtn || !Text_Status) return;

	if (TargetFarm->bIsBreeding)
	{
		float Remaining = TargetFarm->GetRemainingTime();
		float TotalTime = 180.0f;
		float Progress = Remaining / TotalTime;

		PB_EggProgress->SetPercent(Progress);
		PB_EggProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PB_EggProgress->SetPercent(1.0f);
		PB_EggProgress->SetVisibility(ESlateVisibility::Visible);
	}

	if (TargetFarm->bIsEggReady)
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Take")));
		Text_Status->SetText(FText::FromString(TEXT("Egg is Ready!")));
		Btn_Breed->SetIsEnabled(true);
	}
	else if (TargetFarm->bIsBreeding)
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Breeding...")));
		float Remaining = TargetFarm->GetRemainingTime();
		Text_Status->SetText(FText::Format(FText::FromString(TEXT("Next Egg: {0}s")), FMath::CeilToInt(Remaining)));
		Btn_Breed->SetIsEnabled(false);
	}
	else
	{
		Text_BreedBtn->SetText(FText::FromString(TEXT("Start")));

		if (TargetFarm->SpawnedParentA && TargetFarm->SpawnedParentB)
		{
			if (TargetFarm->SpawnedParentA->PalData.PalName == TargetFarm->SpawnedParentB->PalData.PalName)
			{
				Text_Status->SetText(FText::FromString(TEXT("Ready to Breed")));
				Btn_Breed->SetIsEnabled(true);
			}
			else
			{
				Text_Status->SetText(FText::FromString(TEXT("Species Mismatch!")));
				Btn_Breed->SetIsEnabled(false);
			}
		}
		else
		{
			Text_Status->SetText(FText::FromString(TEXT("Waiting for Parents...")));
			Btn_Breed->SetIsEnabled(false);
		}
	}
}

FReply UBreedingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UBreedingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::E)
	{

	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBreedingWidget::OnBreedButtonClicked()
{
	if (!TargetFarm) return;

	if (TargetFarm->bIsEggReady)
	{
		TargetFarm->ClaimEgg(GetOwningPlayerPawn());
	}
	else
	{
		TargetFarm->StartBreeding();
	}
}

void UBreedingWidget::OnFarmStorageUpdated()
{
	if (!TargetFarm || !TargetFarm->FarmStorageComponent || !SB_FarmParents || !PalSlotClass) return;

	SB_FarmParents->ClearChildren();
	int32 TotalCapacity = TargetFarm->FarmStorageComponent->StorageCapacity;

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

		if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerStorageComponent = TargetFarm->FarmStorageComponent;

			FPalData DataToSend;
			if (TargetFarm->FarmStorageComponent->StoredPalsData.IsValidIndex(i))
			{
				DataToSend = TargetFarm->FarmStorageComponent->StoredPalsData[i];
			}
			SlotWidget->SetPalSlotData(DataToSend);
		}
		SB_FarmParents->AddChild(NewSlot);
	}
}

void UBreedingWidget::SetTargetFarm(ABreedingFarm* InFarm)
{
	if (!InFarm || !InFarm->FarmStorageComponent) return;

	TargetFarm = InFarm;

	TargetFarm->FarmStorageComponent->OnPalStorageUpdated.RemoveAll(this);
	TargetFarm->FarmStorageComponent->OnPalStorageUpdated.AddDynamic(this, &UBreedingWidget::OnFarmStorageUpdated);

	OnFarmStorageUpdated();
}

void UBreedingWidget::RefreshPalList()
{
	if (!SB_PlayerPals || !PlayerPalStorageComp.IsValid() || !PalSlotClass) return;

	SB_PlayerPals->ClearChildren();
	int32 TotalCapacity = PlayerPalStorageComp->StorageCapacity;

	for (int32 i = 0; i < TotalCapacity; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, PalSlotClass);

		if (UPalSlotWidget* SlotWidget = Cast<UPalSlotWidget>(NewSlot))
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OwnerStorageComponent = PlayerPalStorageComp.Get();

			FPalData DataToSend;
			if (PlayerPalStorageComp->StoredPalsData.IsValidIndex(i))
			{
				DataToSend = PlayerPalStorageComp->StoredPalsData[i];
			}
			SlotWidget->SetPalSlotData(DataToSend);
		}
		SB_PlayerPals->AddChild(NewSlot);
	}
}