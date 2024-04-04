// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SoldCampCharacter.generated.h"



UCLASS(config=Game)
class ASoldCampCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Walk Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WalkAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Crawl Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrawlAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, 
		Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	/** Mele Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MeleAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, 
		Category = Input,
		meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CallGameMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpeedUpToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CoverAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwapAction2;

public:
	ASoldCampCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void MeleAttacked();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	class UAnimMontage* CoverShootMontage;
	
	bool bAttacking;
	bool bAttackInAttacking;

	//AnimNotify 호출함수
	UFUNCTION(BlueprintCallable)
	void EndAttacking();

	UFUNCTION(BlueprintCallable)
	void EndHitReaction();

	UFUNCTION(BlueprintCallable)
	void AttackInputChecking();

	UFUNCTION(BlueprintCallable)
	void MeleeAttackTracing();

	UFUNCTION(BlueprintCallable)
	void WallTracing();

	UPROPERTY(EditAnywhere)
	class UMeleeAttackState* MyMeleeAttackState;

	void SpeedUpToggleOn();
	void SpeedUpToggleOff();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ClimbLadderToTopEnd = false;

	UPROPERTY(EditAnywhere, Category="Climb")
	double ClimbEndLiftForce = 1.0f;

	void Shoot();
	void Shoot_PlayCoverShootAnim();
	void TriggerCurrentWeapon();
	void Shoot_HitCheck();
	bool Shoot_BulletEmptyCheck();
	void CheckFrontBackHit(FHitResult& Hit, AActor* TargetActor);
	void Shoot_DrawDebugs(FHitResult& Hit);

	void MakeMuzzleFlash(FHitResult& Hit, FVector& ShotDirection);
	void Aimed();
	void Aim_Covered();
	void Aim_CrouchCovered();
	void Aim_Normal();
	void SwitchCameraMode_Aim();
	void SwitchHUD_Character();
	void AlignCharacterForward();

protected:

	/** Called for movement input */
	void Run(const FInputActionValue& Value);
	void RunEnd(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Walk(const FInputActionValue& Value);
	void Crouched(const FInputActionValue& Value);
	void Crawled(const FInputActionValue& Value);
	void AutoFire(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
	void Swaped(const FInputActionValue& Value);
	void Swaped2(const FInputActionValue& Value);
	void CallGameMenu();
	void Interacted();
	void Covered();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	FString SaveSlotName = TEXT("Player1");
	int32 CurrentScore = 0;
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//UPROPERTY(EditAnywhere)
	//class UCapsuleComponent* WallCheckerCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool ShouldCrouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool ShouldCrawl = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool ShouldPunch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool ShouldAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool ShouldClimb = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TakeOver")
	bool ShouldTakeOver = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool Climbable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool Washable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool Coverable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool CoverAimable = false;

	//CrouchCover
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool CrouchCoverable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool CrouchCoverAimable = false;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float CrawlSpeed = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float CapsuleHalfHeight_Prone = 40.0f;

	float CapsuleHalfHeight_Original = 96.0f;
	float CapsuleHalfHeight_Crouch = 60.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AimSpeed = 1.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Damage = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float HeadshootDamage = 100.0f;

	UFUNCTION(BlueprintPure)
	float GetCurrentBullet() const;

	UFUNCTION(BlueprintPure)
	float GetScore();

	void SetScoreUp();
	void SavePlayerData();
	void LoadPlayerData();
	void ResetPlayerData();
	void UpdateMovementMode();

	void CreateCoverBoxChecker();
	void ClimbLadderFinishCheck();

	UPROPERTY(EditAnywhere)
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere)
	class AActor* TargetToPatrol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D MovementVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool CurrentCrouchCover = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* HeadshootZone;

	void SetTargetClimbLocation(AActor* Target);
	FVector GetTargetClimbLocation() const;
	void SetActorLocationToClimbStart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LadderLandable = false;

	void LadderLanding();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LadderClimbDownable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LadderClimbDownStart = false;
	
	UPROPERTY(EditAnywhere)
	float ClimbdownStartLocationOffset = 200.0f;

	FVector ClimbdownStartZoneForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Reloading = false;

	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Dying = false;

	void DelayedDestroy();
	void DelayedHitRecovery();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitFront = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitBack = false;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AGun> GunClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AGun> RifleClass;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float MaxHealth = 100;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float MaxBullet_Pistol = 8;

	float CurrentBullet_Pistol;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MaxBullet_Rifle = 20;

	float CurrentBullet_Rifle;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float WalltraceLength = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TimePerOneShoot = 0.5f;

	int32 CurrentWeaponNum = 0;
	class AGun* MyPistol;
	class AGun* MyRifle;
	class AMyPlayerController* CurrentController;
	FVector* AimTarget;

	FTimerHandle TimerHandle;
	FRotator OriginalRotation;
	float CurrentHealth;

	void DrawAimLine();
	void SetActorRotationToTarget(FHitResult Hit);
	void RotateCharacterToAim();
	void ShowCharacterReturn();
	AActor* CheckInteractiveObject();

	bool ShouldRun;
	float RunSpeed = 0.5f;
	bool CurrentCover = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	double SneakMoveSpeed_l = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	double SneakMoveSpeed_r = 1.0f;

	void ModeChangeToWalking();
	FVector TargetClimbLocation;
	float CurrentDeltaTime = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "HeadShoot")
	class USoundBase* Sfx_HeadShoot;

	class UAnimInstance* CurrentAnimInstance;
};

