// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoldCampCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyPlayerController.h"
#include "Gun.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "CollisionQueryParams.h"
#include "LevelEditorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"
#include "ShowerBooth.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "EnemyAIController.h"


//////////////////////////////////////////////////////////////////////////
// ASoldCampCharacter

ASoldCampCharacter::ASoldCampCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//WallCheckerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WallCheckerCapsule"));
	//WallCheckerCapsule->SetupAttachment(RootComponent);
	//WallCheckerCapsule->OnComponentBeginOverlap.AddDynamic(this, &ASoldCampCharacter::OverlapCheckerCapsule);
	//WallCheckerCapsule->OnComponentEndOverlap.AddDynamic(this, &ASoldCampCharacter::EndOverlapCheckerCapsule);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HeadshootZone = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadshootZone"));
	HeadshootZone->SetupAttachment(RootComponent);
	HeadshootZone->InitCapsuleSize(32.f, 32.0f);

	bAttacking = false;
	bAttackInAttacking = false;
	ShouldRun = false;
	TargetClimbLocation = FVector::Zero();

	CurrentWeaponNum = 0;
}

void ASoldCampCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	MyPistol = GetWorld()->SpawnActor<AGun>(GunClass);
	MyPistol->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	UE_LOG(LogTemp, Warning, TEXT("Pistol Attached.."));
	MyPistol->SetOwner(this);

	MyRifle = GetWorld()->SpawnActor<AGun>(RifleClass);
	MyRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	UE_LOG(LogTemp, Warning, TEXT("Rifle Attached.."));
	//MyRifle->AddActorLocalRotation(FRotator(0, 180, 0));
	//MyRifle->AddActorLocalOffset(FVector(-50.739102f, 3.699009f, 5.450723f));
	MyRifle->SetOwner(this);

	if (CurrentWeaponNum == 0)
	{
		MyPistol->SetActorHiddenInGame(false);
		MyRifle->SetActorHiddenInGame(true);
	}
	else if (CurrentWeaponNum == 1)
	{
		MyPistol->SetActorHiddenInGame(true);
		MyRifle->SetActorHiddenInGame(false);
	}

	CurrentHealth = MaxHealth;
	CurrentBullet_Pistol = MaxBullet_Pistol;
	CurrentBullet_Rifle = MaxBullet_Rifle;

	CurrentController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	FVector Location;
	FRotator Rotation;

	OriginalRotation = RootComponent->GetComponentRotation();

	LoadPlayerData();

	UE_LOG(LogTemp, Display, TEXT("CurrentScore:%d"), CurrentScore);

	CurrentAnimInstance = GetMesh()->GetAnimInstance();;
}

void ASoldCampCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(this->IsPlayerControlled())
	{
		RotateCharacterToAim();
		//CheckInteractiveObject();

		MeleeAttackTracing();

		WallTracing();

		CreateCoverBoxChecker();

		ClimbLadderFinishCheck();

		//UE_LOG(LogTemp, Display, TEXT("Current MovementVector.Y:%lf"), MovementVector.Y);

	}
}

void ASoldCampCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ASoldCampCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ASoldCampCharacter::RunEnd);
		
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASoldCampCharacter::Look);

		//Walking
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ASoldCampCharacter::Walk);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ASoldCampCharacter::Crouched);

		//Crawling
		EnhancedInputComponent->BindAction(CrawlAction, ETriggerEvent::Triggered, this, &ASoldCampCharacter::Crawled);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, 
			ETriggerEvent::Started, this, &ASoldCampCharacter::Shoot);

		EnhancedInputComponent->BindAction(ShootAction, 
			ETriggerEvent::Triggered, this, &ASoldCampCharacter::AutoFire);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, 
			ETriggerEvent::Started, this, &ASoldCampCharacter::Aimed);

		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ASoldCampCharacter::Reload);

		//Calling GameMenu
		EnhancedInputComponent->BindAction(CallGameMenuAction, ETriggerEvent::Started, this, &ASoldCampCharacter::CallGameMenu);
	
		//Interacting
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this, &ASoldCampCharacter::Interacted);

		//Mele Attacking
		EnhancedInputComponent->BindAction(MeleAction, ETriggerEvent::Started, this, &ASoldCampCharacter::MeleAttacked);

		//Mele Attacking
		EnhancedInputComponent->BindAction(SpeedUpToggleAction, ETriggerEvent::Started, this, &ASoldCampCharacter::SpeedUpToggleOn);
		EnhancedInputComponent->BindAction(SpeedUpToggleAction, ETriggerEvent::Completed, this, &ASoldCampCharacter::SpeedUpToggleOff);

		//Covering
		EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Started, this, &ASoldCampCharacter::Covered);

		//Swapping
		EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Started, this, &ASoldCampCharacter::Swaped);
		EnhancedInputComponent->BindAction(SwapAction2, ETriggerEvent::Started, this, &ASoldCampCharacter::Swaped2);
	}

}



void ASoldCampCharacter::RotateCharacterToAim()
{
	if (ShouldAim == true)
	{
		FVector Location;
		FRotator Rotation;

		CurrentController->GetPlayerViewPoint(Location, Rotation);
		FVector Start = Location + Rotation.Vector() * 450.0f;
		FVector End = Location + Rotation.Vector() * 10000.0f;

		FHitResult Hit;
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);


		if (bSuccess)
		{
			FVector ToTarget = End - GetMesh()->GetComponentLocation();

			//FRotator LookAtRotation = FRotator(0, ToTarget.Rotation().Yaw - 75.0f, 0);
			//GetMesh()->SetWorldRotation(LookAtRotation);
			
			this->SetActorRotation(ToTarget.Rotation());
		}
	}
}

void ASoldCampCharacter::ShowCharacterReturn()
{
	this->SetActorHiddenInGame(false);
	MyPistol->SetActorHiddenInGame(false);
	ShouldClimb = false;
}

AActor* ASoldCampCharacter::CheckInteractiveObject()
{
	FVector Location;
	FRotator Rotation;
	CurrentController->GetPlayerViewPoint(Location, Rotation);
	
	FVector Start = Location + Rotation.Vector() * 450.0f;
	FVector End = Location + Rotation.Vector() * 800.0f;
	
	FHitResult Hit;
	
	//bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
	
	bool bSuccess = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(10.0f));

	if (bSuccess == true)
	{
		if (Hit.GetActor()->GetDefaultSubobjectByName(TEXT("ShowerBoothMesh")) != nullptr)
		{
			//UE_LOG(LogTemp, Display, TEXT("Found ShowerBooth..In Check"));
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Found ShowerBooth.."));
			
			return Hit.GetActor();
		}
		else if (Hit.GetActor()->GetDefaultSubobjectByName(TEXT("MyLadderMesh")) != nullptr)
		{
			//UE_LOG(LogTemp, Display, TEXT("Found Ladder..In Check"));
			return Hit.GetActor();
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT("bSuccess == false"));
		return nullptr;
	}
}

float ASoldCampCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageToApply;

	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth:%f"), CurrentHealth);

	if (CurrentHealth <= 0)
	{
		Dying = true;

		if (Cast<AEnemyAIController>(GetController()))
		{
			Cast<AEnemyAIController>(GetController())->SetBlackboardDeadMode();
		}

		if (Sfx_HeadShoot)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sfx_HeadShoot, this->GetActorLocation());
		}

		MyPistol->Destroy();

		if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASoldCampCharacter::DelayedDestroy, 3.0f, false);
		}

		UE_LOG(LogTemp, Warning, TEXT("Target is dead.."));
	}
	else if (CurrentHealth > 0)
	{
		//bHitFront = true;

		if (bHitFront == true || bHitBack == true)
		{
			if (Cast<AEnemyAIController>(GetController()))
			{
				Cast<AEnemyAIController>(GetController())->SetBlackboardHitMode(true);
			}

			if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle) == false)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASoldCampCharacter::DelayedHitRecovery, 1.26f, false);
			}
		}

	}
	
	return DamageToApply;
}

void ASoldCampCharacter::DelayedHitRecovery()
{
	bHitFront = false;
	bHitBack = false;
}

void ASoldCampCharacter::MeleAttacked()
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!CurrentAnimInstance || !AttackMontage)
	{
		return;
	}

	bAttacking = true;

	const char* ComboList[] = { "Combo01","Combo02" };

	if (CurrentAnimInstance->Montage_IsPlaying(AttackMontage) == false)
	{
		//bAttacking = true;
		CurrentAnimInstance->Montage_Play(AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("Montage_Play.."));
	}
	else if (CurrentAnimInstance->Montage_IsPlaying(AttackMontage) == true)
	{
		//bAttacking = true;
		CurrentAnimInstance->Montage_Play(AttackMontage);
		CurrentAnimInstance->Montage_JumpToSection(FName(ComboList[1]), AttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("JumpToSection > Combo02"));
		bAttackInAttacking = true;


	}

}

void ASoldCampCharacter::Walk(const FInputActionValue& Value)
{
	// input is a Vector2D
	//FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y * 0.5f);
		AddMovementInput(RightDirection, MovementVector.X * 0.5f);
	}
}

void ASoldCampCharacter::Crouched(const FInputActionValue& Value)
{
	if (ShouldCrouch == false)
	{
		ShouldCrouch = true;
		ShouldCrawl = false;
		UE_LOG(LogTemp, Warning, TEXT("ShouldCrouch=true"));

		GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight_Crouch);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -60.0f));
	}
	else if(ShouldCrouch == true)
	{
		ShouldCrouch = false;
		UE_LOG(LogTemp, Warning, TEXT("ShouldCrouch=false"));

		GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight_Original);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90.0f));
	}
}

void ASoldCampCharacter::Crawled(const FInputActionValue& Value)
{
	if(ShouldCrouch == true)
	{
		if (ShouldCrawl == false)
		{
			ShouldCrawl = true;
			ShouldCrouch = false;
			UE_LOG(LogTemp, Warning, TEXT("ShouldCrawl=true"));

			GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight_Prone);
			GetMesh()->SetRelativeLocation(FVector(0, 0, -45.0f));
		}
		else
		{
			ShouldCrawl = false;
			UE_LOG(LogTemp, Warning, TEXT("ShouldCrawl=false"));

			GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight_Original);
			GetMesh()->SetRelativeLocation(FVector(0, 0, -90.0f));
		}
	}
}

// by정훈, 발사 동작 시 호출되는 함수
void ASoldCampCharacter::Shoot()
{
	// by정훈, 남은 총알 체크하기위한 변수
	bool isEmptyBullet = false;
	// by정훈, 애니메이션 인스턴스 참조 체크
	if (!CurrentAnimInstance){return;}
	// by정훈, 엄폐사격 애니메이션 참조 체크
	if (!CoverShootMontage){return;}
	// by정훈, 조준 상태 체크
	if (ShouldAim == false){return;}
	// by정훈, 남은 총알 체크
	isEmptyBullet = Shoot_BulletEmptyCheck();
	if (isEmptyBullet == true){return;}
	// by정훈, 엄폐상태일 경우 엄폐사격 애니메이션 플레이
	if (CoverAimable == true){Shoot_PlayCoverShootAnim();}
	// by정훈, 총알 발사
	TriggerCurrentWeapon();
	// by정훈, 라인트레이스 사용하여 명중체크
	Shoot_HitCheck();
}

void ASoldCampCharacter::Shoot_PlayCoverShootAnim()
{
	if (CurrentAnimInstance->Montage_IsPlaying(CoverShootMontage) == false)
	{
		CurrentAnimInstance->Montage_Play(CoverShootMontage);
	}
}

void ASoldCampCharacter::TriggerCurrentWeapon()
{
	if (CurrentWeaponNum == 0)
	{
		MyPistol->GunTriggered();
		CurrentBullet_Pistol--;
		UE_LOG(LogTemp, Display, TEXT("Current Bullet Pistol:%f"), CurrentBullet_Pistol);
	}
	else if (CurrentWeaponNum == 1)
	{
		MyRifle->GunTriggered();
		CurrentBullet_Rifle--;
		UE_LOG(LogTemp, Display, TEXT("Current Bullet Rifle:%f"), CurrentBullet_Rifle);
	}
}

void ASoldCampCharacter::Shoot_HitCheck()
{
	// by정훈, 플레이어 뷰포인트 위치를 저장
	FVector Location;
	// by정훈, 플레이어 뷰포인트 방향을 저장
	FRotator Rotation;
	// by정훈, 라인트레이스 시작점 위치를 저장
	FVector Start;
	// by정훈, 라인트레이스 종료점 위치를 저장
	FVector End;
	// by정훈, 명중 시 피탄효과 방향을 저장
	FVector ShotDirection;
	// by정훈, 명중 대상을 저장
	FHitResult Hit;
	// by정훈, 명중 유무를 저장
	bool bSuccess = false;

	// by정훈, 플레이어캐릭터로 조종되고 있는 경우
	if (this->IsPlayerControlled() == true)
	{
		// by정훈, 플레이어 뷰포트 위치와 방향을 Location, Rotation에 저장
		CurrentController->GetPlayerViewPoint(Location, Rotation);
		// 정훈, 라인트레이스 시작점을 뷰포트 방향으로 8m 진행한 위치로 저장
		Start = Location + Rotation.Vector() * 800.0f;
		// 정훈, 라인트레이스 종료점을 뷰포트 방향으로 100m 진행한 위치로 저장
		End = Location + Rotation.Vector() * 10000.0f;
		// 정훈, 피탄효과 방향을 뷰포트의 반대방향으로 저장
		ShotDirection = -Rotation.Vector();
		// 정훈, 라인트레이스 함수를 호출하여 명중 유무를 저장
		bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End,
			ECollisionChannel::ECC_GameTraceChannel1);
		// 정훈, 플레이어 캐릭터를 발사 방향으로 방향전환 함
		RotateCharacterToAim();
	}
	else if (this->IsPlayerControlled() == false)
	{
		//CurrentController->GetPlayerViewPoint(Location, Rotation);
		Location = this->MyPistol->Mesh->GetSocketLocation(TEXT("MuzzleFlashSocket"));
		Rotation = this->GetActorRotation();
		Start = Location + Rotation.Vector() * 10.0f;
		End = Location + Rotation.Vector() * 1000.0f;
		ShotDirection = -Rotation.Vector();

		bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
	}

	AActor* HitActor = nullptr;
	// by정훈, 불발일 경우 함수 종료
	if (bSuccess == false){return;}
	// by정훈, 총알이 명중일 경우
	else
	{
		// by정훈, 명중 시 총열에서 플래시 이펙트를 생성
		MakeMuzzleFlash(Hit, ShotDirection);
		// by정훈, 명중 시 총알궤적과 피탄지점을 표시
		Shoot_DrawDebugs(Hit);
		// by정훈, 명중시 명중한 Actor를 HitAcot에 저장
		HitActor = Hit.GetActor();
		// by정훈, 명중한 Actor가 존재하고, SoldCampCharacter 일 경우
		if (HitActor != nullptr && Cast<ASoldCampCharacter>(HitActor))
		{
			// by정훈, 명중한 컴포넌트 이름으로 헤드샷을 체크
			if (Hit.GetComponent()->GetName() == TEXT("HeadshootZone"))
			{
				// by정훈, 헤드샷 데미지 이벤트를 생성
				FPointDamageEvent HeadshootDamageEvent(HeadshootDamage, Hit, ShotDirection, nullptr);
				// by정훈, 명중한 액터의 TakeDamage 함수를 호출함
				HitActor->TakeDamage(HeadshootDamage, HeadshootDamageEvent, CurrentController, MyPistol);
				return;
			}
			else
			{
				// by정훈, 데미지이벤트를 생성
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				// by정훈, 명중한 액터의 TakeDamage 함수를 호출함
				HitActor->TakeDamage(Damage, DamageEvent, CurrentController, MyPistol);
			}
			// by정훈, 피탄 각도를 구하여 앞,뒷면 피격를 체크함
			CheckFrontBackHit(Hit, HitActor);
		}
	}
}

bool ASoldCampCharacter::Shoot_BulletEmptyCheck()
{
	if (CurrentWeaponNum == 0 && CurrentBullet_Pistol <= 0)
	{
		MyPistol->PlayEmptySound();
		return true;
	}

	if (CurrentWeaponNum == 1 && CurrentBullet_Rifle <= 0)
	{
		MyRifle->PlayEmptySound();
		return true;
	}

	return false;
}

void ASoldCampCharacter::CheckFrontBackHit(FHitResult& Hit, AActor* TargetActor)
{
	// by정훈, 피탄 부위의 방향값을 저장
	FRotator ToImpactpoint = Hit.ImpactPoint.Rotation();
	// by정훈, 피탄받은 Actor의 전방 방향값 - 타격 부위의 방향값을 ResultRotator에 저장
	FRotator ResultRotator = TargetActor->GetActorForwardVector().Rotation() - ToImpactpoint;
	// by정훈, 명중한 액터가 피탄받은 위치를 향하는 방향값의 각도를 저장
	double ShootAngle = ResultRotator.Yaw;

	// by정훈, 대상이 전면부 각도에서 피격을 받은 경우
	if (ShootAngle < 0 && ShootAngle > -180)
	{
		Cast<ASoldCampCharacter>(TargetActor)->bHitFront = true;
	}
	// by정훈, 대상이 후면부 각도에서 피격을 받은 경우
	else if (ShootAngle >= 0)
	{
		Cast<ASoldCampCharacter>(TargetActor)->bHitBack = true;
	}
}

void ASoldCampCharacter::Shoot_DrawDebugs(FHitResult& Hit)
{
	FVector FireStartLocation = MyPistol->Mesh->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	DrawDebugLine(GetWorld(), FireStartLocation, Hit.ImpactPoint, FColor::Red, false, 2.0f);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10, 12, FColor::Red, false, 2.0f);
}

void ASoldCampCharacter::MakeMuzzleFlash(FHitResult& Hit, FVector& ShotDirection)
{
	UE_LOG(LogTemp, Display, TEXT("Hit target name:%s"), *Hit.GetComponent()->GetName());



	if (HitParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Hit.ImpactPoint, ShotDirection.Rotation());

		//UE_LOG(LogTemp, Warning, TEXT("Controller Rot:%s"), *Rotation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("ShotDir Rot:%s"), *ShotDirection.Rotation().ToString());
	}
}

void ASoldCampCharacter::AutoFire(const FInputActionValue& Value)
{
	if (CurrentWeaponNum == 1)
	{
		CurrentDeltaTime = CurrentDeltaTime + GetWorld()->DeltaTimeSeconds;
		
		if (CurrentDeltaTime > TimePerOneShoot)
		{
			CurrentDeltaTime = 0;
			Shoot();
		}
	}
}

// by정훈, Aim키를 누를 경우 호출 됨
void ASoldCampCharacter::Aimed()
{
	// by정훈, 현재 캐릭터 엄폐상태를 체크함
	if (CurrentCover == true)
	{
		// by정훈, 엄폐 조준 자세로 전환
		Aim_Covered();
		return;
	}
	// by정훈, 현재 캐릭터 무릎앉아 상태를 체크함
	if (CurrentCrouchCover == true)
	{
		// by정훈, 현재 캐릭터 무릎앉아 조준 자세로 전환
		Aim_CrouchCovered();
		return;
	}
	// by정훈, 기본 조준 자세로 전환
	Aim_Normal();
}

void ASoldCampCharacter::Aim_Covered()
{
	if (CoverAimable == false)
	{
		ShouldAim = true;
		CoverAimable = true;
	}
	else if (CoverAimable == true)
	{
		ShouldAim = false;
		CoverAimable = false;
	}
}

void ASoldCampCharacter::Aim_CrouchCovered()
{
	if (CrouchCoverAimable == false)
	{
		ShouldAim = true;
		CrouchCoverAimable = true;
	}
	else if (CrouchCoverAimable == true)
	{
		ShouldAim = false;
		CrouchCoverAimable = false;
	}
}

void ASoldCampCharacter::Aim_Normal()
{
	// by정훈, 캐릭터가 플레이어컨트롤 빙의되지 않은 경우 함수종료.
	if (this->IsPlayerControlled() == false)
	{
		return;
	}
	// by정훈, 현재 조준 상태가 아닌 경우
	if (ShouldAim == false)
	{
		// by정훈, 조준 애니메이션 켜기
		ShouldAim = true;
		// by정훈, 조준 크로스헤어 UI 켜기
		SwitchHUD_Character();
		// by정훈, 카메라 전환 > 조준상태
		SwitchCameraMode_Aim();
	}
	// by정훈, 현재 조준 상태인 경우
	else if (ShouldAim == true)
	{
		// by정훈, 조준 애니메이션 끄기
		ShouldAim = false;
		// by정훈, 조준 크로스헤어 UI 끄기
		SwitchHUD_Character();
		// by정훈, 카메라 전환 > 비 조준상태
		SwitchCameraMode_Aim();
		// by정훈, 조준 방향에 맞춰 캐릭터의 방향전환
		AlignCharacterForward();
	}
}

void ASoldCampCharacter::SwitchCameraMode_Aim()
{
	if (CameraBoom == nullptr)
	{
		return;
	}

	if (ShouldAim == true)
	{
		CameraBoom->TargetArmLength = 200.0f;
		CameraBoom->SocketOffset.Y = 70.0f;
		FollowCamera->SetFieldOfView(60.0f);
		FollowCamera->SetRelativeRotation(FRotator(0, -5, 0));
	}
	else if(ShouldAim == false)
	{
		CameraBoom->TargetArmLength = 400.0f;
		CameraBoom->SocketOffset.Y = 0.0f;
		FollowCamera->SetFieldOfView(90.0f);
		FollowCamera->SetRelativeRotation(FRotator(0, 0, 0));
	}
}

void ASoldCampCharacter::SwitchHUD_Character()
{
	if (CurrentController != nullptr)
	{
		return;
	}

	if (ShouldAim == true)
	{
		CurrentController->TurnOnHUD();
	}
	else
	{
		CurrentController->TurnOffHUD();
	}
}

void ASoldCampCharacter::AlignCharacterForward()
{
	if (CurrentController != nullptr)
	{
		FVector Location;
		FRotator Rotation;
		FVector Start;
		FVector End;
		FHitResult Hit;
		bool bSuccess;

		CurrentController->GetPlayerViewPoint(Location, Rotation);

		Start = Location + Rotation.Vector() * 450.0f;
		End = Location + Rotation.Vector() * 10000.0f;

		bSuccess = GetWorld()->LineTraceSingleByChannel
		(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);

		if (bSuccess == true)
		{
			FVector ToTarget = End - GetMesh()->GetComponentLocation();
			FRotator ToTargetRotation = ToTarget.Rotation();

			FRotator LookAtRotation = FRotator(0, ToTargetRotation.Yaw + 270, 0);
			GetMesh()->SetWorldRotation(LookAtRotation);
		}
	}
}

void ASoldCampCharacter::Reload(const FInputActionValue& Value)
{
	if (CurrentWeaponNum == 0)
	{
		CurrentBullet_Pistol = MaxBullet_Pistol;
		UE_LOG(LogTemp, Display, TEXT("Pistol Reloaded.."));
		UE_LOG(LogTemp, Display, TEXT("Current Bullet Pistol:%f"), CurrentBullet_Pistol);
		Reloading = true;
		MyPistol->PlayReloadSound();
	}
	else if (CurrentWeaponNum == 1)
	{
		CurrentBullet_Rifle = MaxBullet_Rifle;
		UE_LOG(LogTemp, Display, TEXT("Rifle Reloaded.."));
		UE_LOG(LogTemp, Display, TEXT("Current Bullet Rifle:%f"), CurrentBullet_Pistol);
		Reloading = true;
		MyRifle->PlayReloadSound();
	}
}

void ASoldCampCharacter::ReloadEnd()
{
	Reloading = false;
	UE_LOG(LogTemp, Display, TEXT("Reloading ends"));
}

void ASoldCampCharacter::DelayedDestroy()
{
	Destroy();
}

void ASoldCampCharacter::Swaped(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Display, TEXT("Value.ToString:%s"), *Value.ToString());
	//UE_LOG(LogTemp, Display, TEXT("Get magnitude:%f"), Value.GetMagnitude());

	CurrentWeaponNum = 0;

	if (CurrentWeaponNum == 0)
	{
		MyPistol->SetActorHiddenInGame(false);
		MyRifle->SetActorHiddenInGame(true);
	}
	else if (CurrentWeaponNum == 1)
	{
		MyPistol->SetActorHiddenInGame(true);
		MyRifle->SetActorHiddenInGame(false);
	}
}

void ASoldCampCharacter::Swaped2(const FInputActionValue& Value)
{

	//UE_LOG(LogTemp, Display, TEXT("Value.ToString:%s"), *Value.ToString());
	//UE_LOG(LogTemp, Display, TEXT("Get magnitude:%f"), Value.GetMagnitude());
	

	CurrentWeaponNum = 1;

	if (CurrentWeaponNum == 0)
	{
		MyPistol->SetActorHiddenInGame(false);
		MyRifle->SetActorHiddenInGame(true);
	}
	else if (CurrentWeaponNum == 1)
	{
		MyPistol->SetActorHiddenInGame(true);
		MyRifle->SetActorHiddenInGame(false);
	}

}

void ASoldCampCharacter::CallGameMenu()
{
	if (CurrentController != nullptr)
	{
		CurrentController->TuronOnIngameMenu();
	}
}

void ASoldCampCharacter::Interacted()
{
	UE_LOG(LogTemp, Display, TEXT("Interacted.."));

	if (Climbable == true)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		ShouldClimb = true;
		SetActorLocationToClimbStart();
	}
	else if(Climbable == false)
	{
		if (LadderClimbDownable == true)
		{
			ShouldClimb = true;
			LadderClimbDownStart = true;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			UE_LOG(LogTemp, Display, TEXT("Start LadderDown.."));
			SetActorLocationToClimbStart();
		}
		else
		{
			ShouldClimb = false;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}

	if (Washable == true)
	{
		SetActorHiddenInGame(true);
		MyPistol->SetActorHiddenInGame(true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASoldCampCharacter::ShowCharacterReturn, 3.0f, false);
	}
}

void ASoldCampCharacter::LadderLanding()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ASoldCampCharacter::Covered()
{
	UE_LOG(LogTemp, Warning, TEXT("Covered.."));
	UE_LOG(LogTemp, Display, TEXT("Coverable:%d"), Coverable);
	UE_LOG(LogTemp, Display, TEXT("CrouchCoverable:%d"), CrouchCoverable);

	if (Coverable == true && CurrentCover == false)
	{
		ShouldTakeOver = true;
		CurrentCover = true;
		RunSpeed = 0;
	}
	else if (CurrentCover == true)
	{
		ShouldTakeOver = false;
		CurrentCover = false;
		RunSpeed = 0.5f;
	}

	if (CrouchCoverable == true && Coverable == false && CurrentCrouchCover == false)
	{
		CurrentCrouchCover = true;
		RunSpeed = 0;
	}
	else if (CurrentCrouchCover == true)
	{
		CurrentCrouchCover = false;
		RunSpeed = 0.5f;
	}
}

float ASoldCampCharacter::GetCurrentBullet() const
{
	if (CurrentWeaponNum == 0)
	{
		return CurrentBullet_Pistol;
	}
	else if (CurrentWeaponNum==1)
	{
		return CurrentBullet_Rifle;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong current weapon num.."));
		return 0;
	}
}

float ASoldCampCharacter::GetScore()
{
	return CurrentScore;
}

void ASoldCampCharacter::SetScoreUp()
{
	CurrentScore++;
	UE_LOG(LogTemp, Display, TEXT("CurrentScore: %d"), CurrentScore);
	//SavePlayerData();
}

void ASoldCampCharacter::SavePlayerData()
{
	UMySaveGame* SaveDataPtr = NewObject<UMySaveGame>();
	SaveDataPtr->MyScore = CurrentScore;
	UE_LOG(LogTemp, Display, TEXT("SavedScore: %d"), SaveDataPtr->MyScore);

	if (false == UGameplayStatics::SaveGameToSlot(SaveDataPtr, SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save failed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save successed.."));
	}
}

void ASoldCampCharacter::LoadPlayerData()
{
	UMySaveGame* SaveDataPtr = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	
	if (SaveDataPtr == nullptr)
	{
		SaveDataPtr = GetMutableDefault<UMySaveGame>();
		UE_LOG(LogTemp, Warning, TEXT("GetMutableDefault.."));
	}

	CurrentScore = SaveDataPtr->MyScore;
	UE_LOG(LogTemp, Warning, TEXT("Load Succeed.."));
}

void ASoldCampCharacter::ResetPlayerData()
{
	UMySaveGame* SaveDataPtr = NewObject<UMySaveGame>();
	SaveDataPtr->MyScore = 0;

	if (false == UGameplayStatics::SaveGameToSlot(SaveDataPtr, SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save failed.."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save successed.."));
	}
}

void ASoldCampCharacter::UpdateMovementMode()
{
	ShouldClimb = false;
	ClimbLadderToTopEnd = true;
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASoldCampCharacter::ModeChangeToWalking, 1.0f, false);
	}
}

void ASoldCampCharacter::ModeChangeToWalking()
{
	ClimbLadderToTopEnd = false;
	UE_LOG(LogTemp, Warning, TEXT("ModeChangeToWalking.."));
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ASoldCampCharacter::ClimbLadderFinishCheck()
{
	if (ClimbLadderToTopEnd == true)
	{
		//UE_LOG(LogTemp, Display, TEXT("CurrentMovementMode:%s"), *GetCharacterMovement()->GetMovementName());

		if (GetCharacterMovement()->GetMovementName() != TEXT("Flying"))
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}

		this->AddActorLocalOffset(FVector(100.0f, 0, ClimbEndLiftForce) * GetWorld()->DeltaTimeSeconds);

	}
}

void ASoldCampCharacter::SetTargetClimbLocation(AActor* Target)
{
	//UE_LOG(LogTemp, Warning, TEXT("SetTargetClimbLocation.."));
	FVector TargetLocation = Target->GetActorLocation();

	FVector TargetForward = Target->GetActorForwardVector();
	TargetForward *= TargetForward * 50.0f;
	//UE_LOG(LogTemp, Display, TEXT("TargetForward*20:%s"), *TargetForward.ToString());

	//UE_LOG(LogTemp, Display, TEXT("TargetLocation:%s"), *TargetLocation.ToString());
	TargetClimbLocation = FVector(TargetLocation.X, TargetLocation.Y, this->GetActorLocation().Z) + TargetForward;
	
	//UE_LOG(LogTemp, Display, TEXT("TargetClimbLocation%s"), *TargetClimbLocation.ToString());
	
	ClimbdownStartZoneForward = TargetForward * -1;
}

FVector ASoldCampCharacter::GetTargetClimbLocation() const
{

	return TargetClimbLocation;
}

void ASoldCampCharacter::SetActorLocationToClimbStart()
{
	if (LadderClimbDownStart == false)
	{
		SetActorLocation(TargetClimbLocation);
		AddActorLocalOffset(FVector(0, 0, 50.0f));
		//GetMesh()->AddRelativeLocation(FVector(0, 0, 30.0f));
		//HeadshootZone->AddRelativeLocation(FVector(0, 0, 30.0f));
	}
	else
	{
		SetActorLocation(TargetClimbLocation);
		AddActorLocalOffset(FVector(0, 0, ClimbdownStartLocationOffset));
		FRotator ClimbDownStartRotation = ClimbdownStartZoneForward.Rotation();
		AddActorLocalRotation(ClimbDownStartRotation);
	}

}

void ASoldCampCharacter::DrawAimLine()
{
	//AMyPlayerController* CurrentController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	//FVector Location;
	//FRotator Rotation;
	//CurrentController->GetPlayerViewPoint(Location, Rotation);
	//FVector End = Location + Rotation.Vector() * 10000.0f;
	//FHitResult Hit;
	//bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);

	//FVector MuzzleSocketLocation = MyPistol->Mesh->GetSocketLocation(TEXT("MuzzleFlashSocket"));

	//if (bSuccess == true)
	//{
	//	//DrawDebugLine(GetWorld(), MuzzleSocketLocation, Hit.ImpactPoint, FColor::Blue, false, 2.0f);
	//	DrawDebugLine(GetWorld(), GetActorLocation(), Hit.ImpactPoint, FColor::Blue, false, 2.0f);
	//	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10, 12, FColor::Green, false, 2.0f);
	//	DrawDebugSphere(GetWorld(), GetActorLocation(), 10, 12, FColor::Magenta, false, 2.0f);
	//}

	//FVector ToTarget = GetActorLocation() - Hit.ImpactPoint;
	//FRotator LookAtRotation = FRotator(0, ToTarget.Rotation().Yaw, 0);

	//UE_LOG(LogTemp, Display, TEXT("ActorRotation: %s"), *GetActorRotation().ToString());

	//UE_LOG(LogTemp, Display, TEXT("LookAtRotation: %s"), *LookAtRotation.ToString());
	//UE_LOG(LogTemp, Display, TEXT("ForwardRotation: %s"), *ToTarget.ForwardVector.Rotation().ToString());
	//
	//UE_LOG(LogTemp, Display, TEXT("ActorRotation: %s"), *GetActorRotation().ToString());

}

void ASoldCampCharacter::SetActorRotationToTarget(FHitResult Hit)
{

	//AMyPlayerController* CurrentController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	FVector Location;
	FRotator Rotation;

	CurrentController->GetPlayerViewPoint(Location, Rotation);
	FVector Start = Location + Rotation.Vector() * 450.0f;
	FVector End = Location + Rotation.Vector() * 10000.0f;

	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f);

	if (bSuccess == true)
	{
		FRotator ToTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.Location);
		SetActorRelativeRotation(FRotator(0, ToTarget.Yaw + 10.0f, 0));
	}

}

void ASoldCampCharacter::EndAttacking()
{
	UE_LOG(LogTemp, Warning, TEXT("EndAttacking.."));
	bAttacking = false;
	bAttackInAttacking = false;
}

void ASoldCampCharacter::EndHitReaction()
{
	bHitFront = false;
	bHitBack = false;

	if (Cast<AEnemyAIController>(GetController()))
	{
		Cast<AEnemyAIController>(GetController())->SetBlackboardHitMode(bHitFront);
	}

	
}

void ASoldCampCharacter::AttackInputChecking()
{
	if (bAttackInAttacking == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackInput Checking.."));
		MeleAttacked();
		bAttackInAttacking = false;
	}
}

void ASoldCampCharacter::MeleeAttackTracing()
{

	if (bAttackInAttacking == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee Attack Tracing.."));

		FVector Start = GetMesh()->GetSocketLocation(TEXT("WeaponSocket"));
		FVector End = GetMesh()->GetSocketLocation(TEXT("hand_r_Socket_tip"));
		FHitResult OutHit;

		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;

		bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(
			this, Start, End, 20.0f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true);

		if (Result == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit True.."));
		}
	}
}

void ASoldCampCharacter::CreateCoverBoxChecker()
{
	FVector Start = this->GetActorLocation() + this->GetActorForwardVector() * WalltraceLength + FVector(0, 0, -40.0f);
	FVector End = Start + FVector(0, 0, 40.0f);
	FHitResult OutHit;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;
	
	bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, Start, End, 20.0f, ObjectTypes, false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true,
		FLinearColor::Red, FLinearColor::Green, -0.1f);
	
	if (Result == true)
	{
		CrouchCoverable = true;
	}
	else
	{
		CrouchCoverable = false;
	}
}

void ASoldCampCharacter::WallTracing()
{

	//FVector Start_origin = this->GetActorLocation() + GetMesh()->GetForwardVector() * 200.0f;
	//FVector Start_origin = this->GetActorLocation() + this->GetController()->GetControlRotation().Vector() * 200.0f;
	FVector Start_origin_l = this->GetActorLocation()
		+ (GetMesh()->GetComponentRotation() + FRotator(0, 70, 0)).Vector() * WalltraceLength;

	FVector Start_origin_r = this->GetActorLocation()
		+ (GetMesh()->GetComponentRotation() + FRotator(0, 110, 0)).Vector() * WalltraceLength;

	//DrawDebugSphere(GetWorld(), Start_origin, 10, 12, FColor::Red, false, 2.0f);


	//Left side WallTraceSphere
	FVector Start_l = Start_origin_l + FVector(0, 0, 40.0f);

	FVector End_l = Start_l + FVector(0, 0, 40.0f);
	FHitResult OutHit_l;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;

	bool Result_l = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, Start_l, End_l, 20.0f, ObjectTypes, false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit_l, true,
		FLinearColor::Red, FLinearColor::Green, -0.1f);

	//Right side WallTreaceSphere
	FVector Start_r = Start_origin_r + FVector(0, 0, 40.0f);
	FVector End_r = Start_r + FVector(0, 0, 40.0f);
	FHitResult OutHit_r;

	bool Result_r = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, Start_r, End_r, 20.0f, ObjectTypes, false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit_r, true,
		FLinearColor::Red, FLinearColor::Green, -0.1f);

	if (Result_l == true)
	{
		SneakMoveSpeed_l = 1.0f;
	}
	else
	{
		SneakMoveSpeed_l = 0;
	}

	if (Result_r == true)
	{
		SneakMoveSpeed_r = 1.0f;
	}
	else
	{
		SneakMoveSpeed_r = 0;
	}

	if (Result_l == true && Result_r == true)
	{
		Coverable = true;
		//UE_LOG(LogTemp, Warning, TEXT("Coverable:%d"), Coverable);
	}
	else if (Result_l == false || Result_r == false)
	{
		Coverable = false;
		//UE_LOG(LogTemp, Warning, TEXT("Coverable:%d"), Coverable);
	}
}

void ASoldCampCharacter::SpeedUpToggleOn()
{
	UE_LOG(LogTemp, Warning, TEXT("Walk > Run"));
	ShouldRun = true;
	RunSpeed = 1.0f;
}

void ASoldCampCharacter::SpeedUpToggleOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Run > Walk"));
	ShouldRun = false;
	RunSpeed = 0.5f;
}

void ASoldCampCharacter::Run(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// get up vector
		const FVector UpDirection = FVector(ForwardDirection.X, ForwardDirection.Y, 1.0f);
		// add movement 
		if (ShouldClimb == true)
		{
			//AddMovementInput(UpDirection, MovementVector.Y);

			AddMovementInput(FVector::ZeroVector, MovementVector.Y);
			AddMovementInput(FVector::ZeroVector, MovementVector.X);

			if (MovementVector.Y > 0)
			{
				AddActorLocalOffset(FVector(0, 0, 1.0f));
			}
			else if (MovementVector.Y < 0)
			{
				AddActorLocalOffset(FVector(0, 0, -1.0f));
			}

		}
		else if (ShouldCrawl == true)
		{
			AddMovementInput(ForwardDirection, MovementVector.Y * CrawlSpeed);
			AddMovementInput(RightDirection, MovementVector.X * CrawlSpeed);
		}
		else if (ShouldAim == true)
		{
			if (CurrentCover == false)
			{
				AddMovementInput(ForwardDirection, MovementVector.Y * AimSpeed);
				AddMovementInput(RightDirection, MovementVector.X * AimSpeed);
			}
		}
		else if (CurrentCover == true)
		{
			AddMovementInput(FVector::ZeroVector, MovementVector.Y);
			AddMovementInput(FVector::ZeroVector, MovementVector.X);

			if (MovementVector.X > 0)
			{
				AddActorLocalOffset(FVector(0, 1 * SneakMoveSpeed_r, 0));
			}
			else if (MovementVector.X < 0)
			{
				AddActorLocalOffset(FVector(0, -1 * SneakMoveSpeed_l, 0));
			}

		}
		else
		{
			if (MovementVector.X > 0)
			{
				AddMovementInput(ForwardDirection, MovementVector.Y * RunSpeed * 0.5f);
			}
			else
			{
				AddMovementInput(ForwardDirection, MovementVector.Y * RunSpeed);
			}

			if (MovementVector.Y > 0)
			{
				AddMovementInput(RightDirection, MovementVector.X * RunSpeed * 0.5f);
			}
			else
			{
				AddMovementInput(RightDirection, MovementVector.X * RunSpeed);
			}
		}
	}
}

void ASoldCampCharacter::RunEnd(const FInputActionValue& Value)
{
	MovementVector = FVector2D::ZeroVector;

	/*if (CurrentCover == true)
	{
		MovementVector = FVector2D::ZeroVector;
	}*/

}

void ASoldCampCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




