// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Defines.h"
#include "Cart3DStrmln.h"
#include "Visualization.h"
#include "SeederController.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Seeder.generated.h"

// A class to emulate the seeder functionality already present in ActiveFLOVE
UCLASS()
class EXACTIVEFLOVE_API ASeeder : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASeeder();

	Visualization  streamlines; // Member variable connecting Unreal to 

	UPROPERTY(VisibleAnywhere) // location of the Seeder actor
	FVector SdrLocation;

	FVector PreviousActorLocation; // location of the previous seeder actor to keep track of if the actor has moved

	UPROPERTY(VisibleAnywhere, category = "Streamline Points")
	int totolStreamlinePoints;

	UPROPERTY(EditAnywhere, category = "Streamlines") // notifies Unreal of the existance of this object, and it is not able to be Garbage collected by Unreal
	AActor * streamline;

	UPROPERTY(EditAnywhere)
	TArray<FColor> PixelData;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;
	
	FVector CurrentSeederVelocity;
	FVector CurrentCameraVelocity;
	FQuat   CurrentCameraRotation;

	// Create a dynamic texture intended to be used for passing non-texture data
	// into materials. Defaults to 32-bit RGBA. The texture is not added to the
	// root set, so something else will need to hold a reference to it.
	//static UTexture2D* CreateTransientDynamicTexture(int32 Width, int32 Height, EPixelFormat PixelFormat = PF_FloatRGB);
	//// Updates a region of a texture with the supplied input data. Does nothing
	//// if the pixel formats do not match.
	//static void UpdateTextureRegion(UTexture2D* Texture, int32 MipIndex, FUpdateTextureRegion2D Region, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);
	//// Convenience wrapper for updating a dynamic texture with an array of
	//// FLinearColors.
	//static void UpdateDynamicVectorTexture(const TArray<FLinearColor>& Source, UTexture2D* Texture);
	//// Sets up a component's material instance parameters (on all materials) for
	//// use with the supplied UTexture. The proper parameters (specified by
	//// IndexParameterName and TextureParameterName) should exist on the
	//// material, otherwise this will not have the proper effect.
	//static void SetDynamicTextureAndIndex(class UStaticMeshComponent* Component, class UTexture2D* Texture, int32 Index, FName IndexParameterName, FName TextureParameterName);

	int SdrGrdRes;
	int SeederResolution;

	// For Movement in the environment --------------

	void MoveSeeder_XAxis(float AxisValue);
	void MoveSeeder_YAxis(float AxisValue);
	void MoveSeeder_ZAxis(float AxisValue);

	void MoveCamera_XAxis(float AxisValue);
	void MoveCamera_YAxis(float AxisValue);
	void MoveCamera_ZAxis(float AxisValue);
	void RotateCameraYaw(float AxisValue);
	void RotateCameraPitch(float AxisValue);
	void RotateCameraRoll(float AxisValue);

	void ReCenterCamera  (float AxisValue);

	// ----------------------------------------------

	virtual void OnConstruction(const FTransform & Transform);

	void RenderStreamlines ();
	void RenderStreamline  (UCRGBPT*  pPoints, int  clrIndx, int strmnumber);
	void SetUserLocation   ();
	void AdjustUserLocation();
	void RunActiveFLOVE    ();
	void Uninitialize	   ();
	//void CreateTexture(UTexture2D * Texture2D);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
};

