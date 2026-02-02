// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// "LogSparta"라는 이름으로 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(LogSparta, Warning, All);

UCLASS()
class SPARTAPROJECT_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

protected:
	// 루트 컴포넌트를 나타내는 Scene Component 포인터
	USceneComponent* SceneRoot;
	// Static Mesh Component 포인터
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
};
