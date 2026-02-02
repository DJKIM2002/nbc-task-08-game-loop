// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent; // 스프링 암 관련 클래스 헤더
class UCameraComponent;	   // 카메라 관련 클래스 전방 선언
class UWidgetComponent;	   // 위젯 컴포넌트 전방 선언

// Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	// 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	// 현재 체력을 가져오는 함수
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;
	// 체력을 회복시키는 함수
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(int32 Amount);

protected:
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;
  // 체력바 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> OverheadWidget;

	virtual void BeginPlay() override;
	
  // 사망 처리 함수 (체력 0 이하 시 호출)
	void OnDeath();
	
  // 체력바 위젯 업데이트 함수
	void UpdateOverheadHP();

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	// 데미지 처리 함수 - 외부로부터 데미지를 받을 때 호출
	// 또는 AActor의 TakeDamage()를 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInsigator, AActor* DamageCauser) override;

private:
	// 이동 속도 관련 속성
	float NormalSpeed;			 // 기본 걷기 속도
	float SprintSpeedMultiplier; // 달리기 속도 배율
	float SprintSpeed;			 // 달리기 속도
};
