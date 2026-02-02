// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class SPARTAPROJECT_API IItemInterface
{
	GENERATED_BODY()

public:
	// 플레이어가 이 아이템의 범위에 들어왔을 때 호출
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
		= 0;
	// 플레이어가 이 아이템의 범위에서 벗어났을 때 호출
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex)
		= 0;
	// 아이템이 사용되었을 때 호출
	virtual void ActivateItem(AActor* Activator) = 0;
	// 이 아이템의 유형(타입)을 반환
	virtual FName GetItemType() const = 0;
};
