// Fill out your copyright notice in the Description page of Project Settings.

#include "SpartaCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SpartaPlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "SpartaGameState.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	// 캐릭터와 카메라 사이의 거리 기본값 300 설정
	SpringArmComp->TargetArmLength = 300.f;
	// 컨트롤러 회전에 따라 스프링 암도 회전하도록 설정
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// 스프링 암의 소켓 위치에 카메라를 부착
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	// 카메라는 스프링 암의 회전을 따르므로 PawnControlRotation은 꺼둠
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

int32 ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(int32 Amount)
{
	// 체력을 회복, 최대 체력을 초과하지 않도록 제한
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced InputComponent로 캐스팅
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	// 컨트롤러가 있어야 방향 계산 가능
	if (!Controller)
		return;

	// Value는 Axis2D로 설정된 IA_Move의 입력값 (WASD)을 담고 있음
	// X = 1, Y = 0 -> 앞 (W)
	// X = -1, Y = 0 -> 뒤 (S)
	// X = 0, Y = -1 -> 왼쪽 (A)
	// X = 0, Y = 1 -> 오른쪽 (D)
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// 캐릭터가 바라보는 방향(정면)으로 X축 이동
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// 캐릭터의 오른쪽 방향으로 Y축 이동
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	// Jump 함수는 Character가 기본 제공
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	// StopJumping 함수도 Character가 기본 제공
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	// 마우스의 X, Y 움직임을 2D 벡터로 받아옴
	FVector2D LookInput = value.Get<FVector2D>();

	// X는 좌우 회전 (Yaw), Y는 상하 회전 (Pitch)
	// 좌우 회전
	AddControllerYawInput(LookInput.X);
	// 상하 회전
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	// 달리기 속도 적용
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	// 기본 걷기 속도 적용
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInsigator, AActor* DamageCauser)
{
	// 기본 데미지 처리 로직 호출
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInsigator, DamageCauser);
	// 체력을 데미지만큼 감소시키고, 0 이하로 떨어지지 않도록 Clamp
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	UpdateOverheadHP();

	// 체력이 0 이하가 되면 사망 처리
	if (Health <= 0.f)
	{
		OnDeath();
	}
	// 실제 적용된 데미지 반환
	return ActualDamage;
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget)
		return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance)
		return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}