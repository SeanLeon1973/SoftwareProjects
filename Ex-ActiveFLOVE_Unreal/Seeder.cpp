// Fill out your copyright notice in the Description page of Project Settings.

#include "Seeder.h"

#include "Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h" 
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Public/TextureResource.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstance.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"

//#include ""

#ifndef _STREAM_HEADER_
#define _STREAM_HEADER_
typedef struct tagSTREAMHEADER
{
	int   	points;									///Number of on-streamline POINTS
	int 	sampls;									///Number of SAMPLeS
	int		sIndex;									///Seed-sample INDEX, in terms of SAMPLE or POINT3D, relative to
													///The entry of a sample-array (but NOT that of the streamline array)
	tagSTREAMHEADER() {}
	tagSTREAMHEADER(int  ponts, int  samps, int  index) { points = ponts;  sampls = samps;  sIndex = index; }
}	STREAMHEADER;
#endif

#ifndef _SAMPLE_
#define _SAMPLE_
typedef struct tagSAMPLE // orignally from Cart2Dstrmer
{
	POINT3D	 point;
	VECTOR3D vectr;

	tagSAMPLE() {}
	tagSAMPLE(POINT3D  p, VECTOR3D  v) { point = p;  vectr = v; }
}	SAMPLE;
#endif

FName names[125]{ "streamlines0"  ,"streamlines1"  ,"streamlines2"  ,"streamlines3"  ,"streamlines4"  ,"streamlines5"  ,"streamlines6"  ,"streamlines7"  ,"streamlines8"  ,"streamlines9"  ,
							  "streamlines10" ,"streamlines11" ,"streamlines12" ,"streamlines13" ,"streamlines14" ,"streamlines15" ,"streamlines16" ,"streamlines17" ,"streamlines18" ,"streamlines19" ,
							  "streamlines20" ,"streamlines21" ,"streamlines22" ,"streamlines23" ,"streamlines24" ,"streamlines25" ,"streamlines26" ,"streamlines27" ,"streamlines28" ,"streamlines29" ,
							  "streamlines30" ,"streamlines31" ,"streamlines32" ,"streamlines33" ,"streamlines34" ,"streamlines35" ,"streamlines36" ,"streamlines37" ,"streamlines38" ,"streamlines39" ,
							  "streamlines40" ,"streamlines41" ,"streamlines42" ,"streamlines43" ,"streamlines44" ,"streamlines45" ,"streamlines46" ,"streamlines47" ,"streamlines48" ,"streamlines49" ,
							  "streamlines50" ,"streamlines51" ,"streamlines52" ,"streamlines53" ,"streamlines54" ,"streamlines55" ,"streamlines56" ,"streamlines57" ,"streamlines58" ,"streamlines59" ,
							  "streamlines60" ,"streamlines61" ,"streamlines62" ,"streamlines63" ,"streamlines64" ,"streamlines65" ,"streamlines66" ,"streamlines67" ,"streamlines68" ,"streamlines69" ,
							  "streamlines70" ,"streamlines71" ,"streamlines72" ,"streamlines73" ,"streamlines74" ,"streamlines75" ,"streamlines76" ,"streamlines77" ,"streamlines78" ,"streamlines79" ,
							  "streamlines80" ,"streamlines81" ,"streamlines82" ,"streamlines83" ,"streamlines84" ,"streamlines85" ,"streamlines86" ,"streamlines87" ,"streamlines88" ,"streamlines89" ,
							  "streamlines90" ,"streamlines91" ,"streamlines92" ,"streamlines93" ,"streamlines94" ,"streamlines95" ,"streamlines96" ,"streamlines97" ,"streamlines98" ,"streamlines99" ,
							  "streamlines100","streamlines101","streamlines102","streamlines103","streamlines104","streamlines105","streamlines106","streamlines107","streamlines108","streamlines109",
							  "streamlines110","streamlines111","streamlines112","streamlines113","streamlines114","streamlines115","streamlines116","streamlines117","streamlines118","streamlines119",
							  "streamlines120","streamlines121","streamlines122","streamlines123","streamlines124" };

//// Sets default values //
ASeeder::ASeeder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent * RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); // CreateDefualtSubobject can only be used in constructor use newobject when not using constructor

	RootComponent->SetMobility(EComponentMobility::Movable);

	RunActiveFLOVE();

	this->SetRootComponent(RootComponent);

	SdrGrdRes = 5;
	SeederResolution = SdrGrdRes * SdrGrdRes * SdrGrdRes;
	SdrLocation.X = (streamlines.getNXRES() >> 1);
	SdrLocation.Y = (streamlines.getNYRES() >> 1);
	SdrLocation.Z = (streamlines.getNZRES() >> 1);

	FVector Temp = SdrLocation * 50;
	this->SetActorLocation(Temp);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Camera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	Camera->SetOrthoFarClipPlane( 1000.0f );

	AutoPossessPlayer = EAutoReceiveInput::Player0; // May not be nessecary due to Unpossess() and Possess located in BeginPlay() function
}

void ASeeder::Uninitialize()
{
	this->GetWorld()->PersistentLineBatcher->Flush(); // Removes old lines 
	streamline->Destroy();  
	PixelData.Empty();	
}

void ASeeder::OnConstruction(const FTransform & Transform)
{
	//Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Streamline_Tube")));
	Uninitialize(); // removes old streamlines if there is any
	//if (SdrLocation != PreviousSeederLocation) // does not remove the orginal streamline when the editor starts
	//{
	//	Uninitialize(); // removes old seeder
	//	PreviousSeederLocation = SdrLocation;
	//}

	SetUserLocation();
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void ASeeder::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	SetUserLocation();
}

// Called every frame
void ASeeder::Tick(float DeltaTime)
{
	// Handle movement of pawn
	// Is the user asking the seeder to move?
	if (!CurrentSeederVelocity.IsZero())
	{
		FVector NewLocation = this->GetActorLocation() + (CurrentSeederVelocity * DeltaTime);
		SetActorLocation(NewLocation);
	}
	else if (!CurrentCameraVelocity.IsZero()) // is the camera moving?
	{						  // Velocity * time = displacment
		FVector NewLocation = (CurrentCameraVelocity * DeltaTime);
		//FVector NewLocation = Camera->GetComponentLocation() + (CurrentCameraVelocity * DeltaTime);
		Camera->SetRelativeLocation(NewLocation);
		//Camera->AddLocalRotation(CurrentCameraRotation);
		Camera->SetRelativeRotation(CurrentCameraRotation);
	}
	else // This only shows the seeder when the keys are not pressed anymore to speed up performance. 
	{
		if (this->GetActorLocation() != PreviousActorLocation)
		{
			Uninitialize(); // removes old seeder
			SetUserLocation();
			PreviousActorLocation = this->GetActorLocation();
		}
	}

	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASeeder::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->Priority = 20;

	// Seeder Controls ------------------------------------------------------------------------
																					 // All directions in relation to axis in question
	PlayerInputComponent->BindAxis("MoveSeederX", this, &ASeeder::MoveSeeder_XAxis); // Bound to W(right) S(left)  x-axis
	PlayerInputComponent->BindAxis("MoveSeederY", this, &ASeeder::MoveSeeder_YAxis); // Bound to A(left) D (right) y-axis
	PlayerInputComponent->BindAxis("MoveSeederZ", this, &ASeeder::MoveSeeder_ZAxis); // Bound to Q(up) E(down)     z-axis
	// Camera Controls ------------------------------------------------------------------------
	PlayerInputComponent->BindAxis("MoveCameraX", this, &ASeeder::MoveCamera_XAxis  ); // Not working yet 
	PlayerInputComponent->BindAxis("MoveCameraY", this, &ASeeder::MoveCamera_YAxis  ); // Not working yet 
	PlayerInputComponent->BindAxis("MoveCameraZ", this, &ASeeder::MoveCamera_ZAxis  ); // Not working yet 
	PlayerInputComponent->BindAxis("RotateCameraRoll" , this, &ASeeder::RotateCameraRoll  );
	PlayerInputComponent->BindAxis("RotateCameraPitch", this, &ASeeder::RotateCameraPitch );
	PlayerInputComponent->BindAxis("RotateCameraYaw"  , this, &ASeeder::RotateCameraYaw   );

	// Other Controls -------------------------------------------------------------------------
	PlayerInputComponent->BindAxis("ReCenterCamera", this, &ASeeder::ReCenterCamera); // Not working yet
	// ----------------------------------------------------------------------------------------

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//	sets the speed at which you move based on the float value it is multiplied by ---------------------------------------------
// Speed along the X axis
void ASeeder::MoveSeeder_XAxis(float AxisValue)
{
	CurrentSeederVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f; // either returns a 0 or 1 or -1 
}
// Speed along the Y axis
void ASeeder::MoveSeeder_YAxis(float AxisValue)
{
	CurrentSeederVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f;
}
//Speed along the Z Axis
void ASeeder::MoveSeeder_ZAxis(float AxisValue)
{
	CurrentSeederVelocity.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f;
}

void ASeeder::MoveCamera_XAxis(float AxisValue)
{
	CurrentCameraVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f;
}
void ASeeder::MoveCamera_YAxis(float AxisValue)
{
	CurrentCameraVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f;
}
void ASeeder::MoveCamera_ZAxis(float AxisValue)
{
	CurrentCameraVelocity.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 300.0f;
}

// Around x axis
void ASeeder::RotateCameraRoll(float AxisValue)
{
	CurrentCameraRotation.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 10.0f;
}

// Around y axis
void ASeeder::RotateCameraPitch(float AxisValue)
{
	CurrentCameraRotation.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 10.0f;
}

// Around z axis
void ASeeder::RotateCameraYaw(float AxisValue)
{
	CurrentCameraRotation.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 10.0f;
}

void ASeeder::ReCenterCamera(float AxisValue)
{
	if (AxisValue)
	{
		Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
}
// ----------------------------------------------------------------------------------------------------------------------------

// Used for adjusting the Seeder location after the program has started
void ASeeder::AdjustUserLocation()
{
	this->SetActorLocation((SdrLocation * 50));

	RunActiveFLOVE();    // Computation
	RenderStreamlines(); // Rendering
}


// Sets the user location and renders the streamlines
void ASeeder::SetUserLocation()
{
	Uninitialize(); // Removes any extra streamlines
	SeederResolution = SdrGrdRes * SdrGrdRes * SdrGrdRes;

	FVector Userlocation = this->GetActorLocation();

	Userlocation.X /= 50;
	Userlocation.Y *= -1; // set to positive as unreal world space y is negative in our case  ( x, -y, z ) coordinate system
	Userlocation.Y /= 50;
	Userlocation.Z /= 50;
	SdrLocation = Userlocation;


	// Unreal Coordinate system
	//
	//			     z
	//               |
	//				 |   x
	//               |  /
	//				 | /
	//				 |/
	// -y -----------+
	//



	RunActiveFLOVE();    // Computation
	RenderStreamlines(); // Rendering
}

//Runs ActiveFLOVE
void ASeeder::RunActiveFLOVE()
{
	streamlines.setEYEPT(POINT3D(SdrLocation.X, SdrLocation.Y, SdrLocation.Z));
	streamlines.Init3DStreamlines();
}

//Renders all seeded streamlines
void	ASeeder::RenderStreamlines()
{
	streamline = NewObject<AActor>(this);
	streamline->SetActorTransform(this->GetActorTransform()); // may not be needed not sure if Aseeder passes its root component
	UCRGBPT** PPoints = nullptr;

	//streamline->RemoveInstanceComponent(); // need to add incase problem of old splines not being deleted shows up again in play mode	
	//clearsplinepoints

	PPoints = streamlines.p3Dstrm->GetStreamlinesArray();
	if (PPoints)
	{
		for (int i = 0; i < SeederResolution; i++)
		{
			RenderStreamline(PPoints[i], i % RANDOM_COLORS, i);
		}
	}
	PPoints = nullptr;
}

// Renders a single streamline
void ASeeder::RenderStreamline(UCRGBPT*  pPoints, int  clrIndx, int strmnumber)
{
	STREAMHEADER * pHeader = (STREAMHEADER*)pPoints;
	SAMPLE       * pSample = (SAMPLE*)(pPoints + pHeader->points + 1);

	totolStreamlinePoints = pHeader->points; // Functions as a purely visual asset and has no affect on the outcome of the result

	FColor Color;

	FVector StartLocation, StartTangent;
	FVector EndLocation, EndTangent;

	// Creates one streamline
	// I initially set to two due to USplineComponent constructor requiring a point to start
	// Spline Loop must be called before mesh loop as i + 1 
	for (int i = 1; i <= pHeader->points; i++) // Spline loop
	{
		// Scaling each point to their global location
		FVector Point        (pPoints[i].point.x * 50, -1 * pPoints[i].point.y * 50, pPoints[i].point.z * 50);
		FVector PreviousPoint(pPoints[i - 1].point.x * 50, -1 * pPoints[i - 1].point.y * 50, pPoints[i - 1].point.z * 50);

		// To avoid having points set at the locations of the seed point when out of the flow data
		if ( Point.X < 0.0 || Point.X >  18000 ) { continue; } // to remove the points added at the seed point when outside the boundaries 
		if ( Point.Y > 0.0 || Point.Y < -9050  ) { continue; }
		if ( Point.Z < 0.0 || Point.Z >  650   ) { continue; }

		Color.R = pPoints[i].color.R;
		Color.G = pPoints[i].color.G;
		Color.B = pPoints[i].color.B;
		Color.A = 1.0f;

		// lines will start at zero at i = 1
		if ( i != 1 )
		{
			this->GetWorld()->PersistentLineBatcher->DrawLine(PreviousPoint, Point, Color, 0, 0.1f);
		}
	}

	/// In order to fix the unblended colors in the streamlines
	//UStaticMesh* Mesh;

	//Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/VirtualReality/Meshes/StreamlineTube.StreamlineTube"))); // MUST!!! be here 

	//// This loop handles making meshes at each point along the streamline minus the last one which does not need a mesh
	//// Also, it handles coloring the meshes
	//for (int i = 0; i < (spline->GetNumberOfSplinePoints() - 1); i++) // Mesh loop
	//{
	//	// Handling the Mesh --------------------------------------
	//	USplineMeshComponent * tube = NewObject<USplineMeshComponent>(spline, USplineMeshComponent::StaticClass());

	//	tube->SetMobility(EComponentMobility::Static); // Option: Static: lighting can be baked, Stationary: lighting done at runtime, Movable: Object is
	//												   // movable within the scene (dynamic lighting) lighting recalculated when moved
	//	tube->SetStaticMesh(Mesh);
	//	tube->SetForwardAxis(ESplineMeshAxis::Type::Z, true); // This is because Z access is parallel with line

	//	UMaterialInterface * Material = tube->GetMaterial( 0 ); // Gets the material currently being used for the mesh

	//	//Material->CheckMaterialUsage(EMaterialUsage::MATUSAGE_SplineMesh); // used for instanced static meshes
	//	UMaterialInstanceDynamic* matInstance = tube->CreateDynamicMaterialInstance(0, Material); // creates a dynamic instance of that material
	//	//UMaterial * Mat;
	//	//MaterialLayerBlend

	//	//matInstance != nullptr

	//	if (Material->IsValidLowLevelFast()) // if this material exists modify it based on data provided by ActiveFLOVE
	//	{
	//		matInstance->SetVectorParameterValue(FName(TEXT("BaseColor")), PixelData[i]);
	//		matInstance->TwoSided = 0;
	//		//matInstance->SetScalarParameterValue(FName(TEXT("Roughness")), 0.9f);
	//		//matInstance->SetScalarParameterValue(FName(TEXT("Metallic")), 0.1f);
	//		//matInstance->BlendMode = EBlendMode::BLEND_Translucent;
	//		//matInstance->ShadingModel = EMaterialShadingModel::MSM_Unlit;
	//	}

	//	tube->SetMaterial(0, matInstance);

	//	spline->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::Type::World);
	//	spline->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::Type::World);

	//	tube->bAffectDistanceFieldLighting = 0;

	//	tube->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
	//	tube->RegisterComponent();
	//}
	//
	//streamline->AttachToComponent(spline, FAttachmentTransformRules::KeepRelativeTransform); // xxx spline may not keep data that is put into it
	pHeader = nullptr;
	pSample = nullptr;
}


// Use this function to update the texture rects you want to change:
// NOTE: There is a method called UpdateTextureRegions in UTexture2D but it is compiled WITH_EDITOR and is not marked as ENGINE_API so it cannot be linked from plugins.

//void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
//{
//	if (Texture->Resource)
//	{
//		struct FUpdateTextureRegionsData
//		{
//			FTexture2DResource* Texture2DResource;
//			int32 MipIndex;
//			uint32 NumRegions;
//			FUpdateTextureRegion2D* Regions;
//			uint32 SrcPitch;
//			uint32 SrcBpp;
//			uint8* SrcData;
//		};
//
//		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
//
//		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
//		RegionData->MipIndex = MipIndex;
//		RegionData->NumRegions = NumRegions;
//		RegionData->Regions = Regions;
//		RegionData->SrcPitch = SrcPitch;
//		RegionData->SrcBpp = SrcBpp;
//		RegionData->SrcData = SrcData;
//
//		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
//			UpdateTextureRegionsData,
//			FUpdateTextureRegionsData*, RegionData, RegionData,
//			bool, bFreeData, bFreeData,
//			{
//			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
//			{
//				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
//				if (RegionData->MipIndex >= CurrentFirstMip)
//				{
//					RHIUpdateTexture2D(
//						RegionData->Texture2DResource->GetTexture2DRHI(),
//						RegionData->MipIndex - CurrentFirstMip,
//						RegionData->Regions[RegionIndex],
//						RegionData->SrcPitch,
//						RegionData->SrcData
//						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
//						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
//						);
//				}
//			}
//			if (bFreeData)
//			{
//				FMemory::Free(RegionData->Regions);
//				FMemory::Free(RegionData->SrcData);
//			}
//			delete RegionData;
//			});
//	}
//}

//
//void ASeeder::UpdateTextureRegion(UTexture2D* Texture, int32 MipIndex, FUpdateTextureRegion2D Region, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
//{
//	if (Texture->Resource)
//	{
//		struct FUpdateTextureRegionsData
//		{
//			FTexture2DResource* Texture2DResource;
//			int32 MipIndex;
//			FUpdateTextureRegion2D Region;
//			uint32 SrcPitch;
//			uint32 SrcBpp;
//			uint8* SrcData;
//		};
//
//		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
//
//		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
//		RegionData->MipIndex = MipIndex;
//		RegionData->Region = Region;
//		RegionData->SrcPitch = SrcPitch;
//		RegionData->SrcBpp = SrcBpp;
//		RegionData->SrcData = SrcData;
//
//		{
//
//			ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
//				UpdateTextureRegionsData,
//				FUpdateTextureRegionsData*, RegionData, RegionData,
//				bool, bFreeData, bFreeData,
//				{
//					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
//					if (RegionData->MipIndex >= CurrentFirstMip)
//					{
//						RHIUpdateTexture2D(
//							RegionData->Texture2DResource->GetTexture2DRHI(),
//							RegionData->MipIndex - CurrentFirstMip,
//							RegionData->Region,
//							RegionData->SrcPitch,
//							RegionData->SrcData
//							+ RegionData->Region.SrcY * RegionData->SrcPitch
//							+ RegionData->Region.SrcX * RegionData->SrcBpp
//							);
//					}
//					// TODO is this leaking if we never set this to true??
//					if (bFreeData)
//					{
//						FMemory::Free(RegionData->SrcData);
//					}
//					delete RegionData;
//				});
//
//		}
//	}
//}
//
//void ASeeder::UpdateDynamicVectorTexture(const TArray<FLinearColor>& Source, UTexture2D* Texture)
//{
//	// Only handles 32-bit float textures
//	if (!Texture || Texture->GetPixelFormat() != PF_A32B32G32R32F) return;
//	// Shouldn't do anything if there's no data
//	if (Source.Num() < 1) return;
//
//	UpdateTextureRegion(Texture, 0, FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY()), Texture->GetSizeX() * sizeof(FLinearColor), sizeof(FLinearColor), (uint8*)Source.GetData(), false);
//}
//
//UTexture2D* ASeeder::CreateTransientDynamicTexture(int32 Width, int32 Height, EPixelFormat PixelFormat /*= PF_A32B32G32R32F*/)
//{
//	auto* Texture = UTexture2D::CreateTransient(Width, Height, PixelFormat);
//	if (Texture)
//	{
//		Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//		Texture->SRGB = 0;
//		Texture->UpdateResource();
//	}
//	return Texture;
//}
//
//void ASeeder::SetDynamicTextureAndIndex(class UStaticMeshComponent* Component, class UTexture2D* Texture, int32 Index, FName IndexParameterName, FName TextureParameterName)
//{
//	//if (!Component || !Texture) return;
//	//for (int32 i = 0; i < Component->GetNumMaterials(); i++)
//	//{
//	//	UMaterialInstanceDynamic* DynamicMaterial = TryGetDynamicMaterial(Component, i);
//	//	if (!DynamicMaterial) continue;
//	//	FLinearColor CalculatedIndex(FMath::Fmod((float)Index, 64.0f) + 0.5f, FMath::FloorToFloat((float)Index / 64.0f) + 0.5f, 0.0f, 0.0f);
//	//	CalculatedIndex /= 64.0f;
//	//	DynamicMaterial->SetVectorParameterValue(IndexParameterName, CalculatedIndex);
//	//	DynamicMaterial->SetTextureParameterValue(TextureParameterName, Texture);
//	//}
//}


//void ASeeder::CreateTexture(UTexture2D * Texture2D)
//{
	//if (TheMaterial == NULL)
	//{
	//	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, FString("AMenuButton::CreateTextTexture ButtonMaterial == NULL, text = " + TextToDraw));
	//	return;
	//}
	//UTextureRenderTarget2D* Texture = NewObject<UTextureRenderTarget2D>(Texture2D);
	//Texture->InitAutoFormat(256, 128);
	//Texture->bNeedsTwoCopies = false;
	//Texture->bHDR_DEPRECATED = false;
	//Texture->ClearColor = FLinearColor::Black;
	//Texture->UpdateResourceImmediate();

	////FCanvas Canvas = FCanvas(Texture->GameThread_GetRenderTargetResource(), NULL, this->GetWorld(), GMaxRHIFeatureLevel);
	////Canvas.Clear(FLinearColor::Black);
	////Texture->UpdateResourceImmediate(false);

	////FText ThisText = FText::FromString(TextToDraw);
	////FFontRenderInfo RenderInfo = FFontRenderInfo();
	////RenderInfo.bClipText = true;
	////FCanvasTextItem TextItem(FVector2D(0.0, 0.0), ThisText, Font, FLinearColor::White);
	////TextItem.Scale = TextScale,
	////TextItem.BlendMode = SE_BLEND_Translucent;  look at this more
	////TextItem.FontRenderInfo = RenderInfo;
	////Canvas.DrawItem(TextItem);

	//// TODO: Center the text, using TextItem.DrawnSize which will tell us how big it is - maybe use the material editor and put an offset in there;

	//TheMaterial->SetTextureParameterValue(TEXT("TextTexture"), Texture);

	////Canvas.Flush_GameThread();
	////GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Updated button text to : "+TextToDraw));
//}
