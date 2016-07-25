// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AllowWindowsPlatformTypes.h"
#include <Kinect.h>
#include <Kinect.Face.h>
#include "HideWindowsPlatformTypes.h"
#include <windows.h>
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"


UCLASS()
class FACE_API AMyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	float RunningTime;

	UFUNCTION(BlueprintCallable, Category = "Custom")
		float GetMouthLeft(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Custom")
		float GetMouthRight(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, category = "Custom")
		bool IsKinectReady();

	

	float GetMouth(float DeltaSeconds);

	// Kinect
	static const int       cColorWidth = 1920;
	static const int       cColorHeight = 1080;
	bool KinectReady;
	bool happyDetected;
	/*HRESULT GetDefaultSensor();
	void update();
	HRESULT updateBodyData(IBody** ppBodies);
	void processFaces();*/
	template<class Interface>
	void SafeRelease(Interface *& pInterfaceToRelease)
	{
		if (pInterfaceToRelease != nullptr)
		{
			pInterfaceToRelease->Release();
			pInterfaceToRelease = nullptr;
		}
	}
	/*IKinectSensor*         KinectSensor;
	ICoordinateMapper*     CoordinateMapper;
	IColorFrameReader*     ColorFrameReader;
	IBodyFrameReader*      BodyFrameReader;
	IFaceFrameSource*	   FaceFrameSources[BODY_COUNT];
	IFaceFrameReader*	   FaceFrameReaders[BODY_COUNT];
	RGBQUAD*               ColorRGBX;
	void                   Update();*/

	HRESULT                InitializeDefaultSensor();
	void				   Update();
	void                   ProcessFaces();
	HRESULT                UpdateBodyData(IBody** ppBodies);

	HWND                   m_hWnd;
	INT64                  m_nStartTime;
	INT64                  m_nLastCounter;
	double                 m_fFreq;
	ULONGLONG              m_nNextStatusTime;

	// Current Kinect
	IKinectSensor*         m_pKinectSensor;
	// Coordinate mapper
	ICoordinateMapper*     m_pCoordinateMapper;
	// Color reader
	IColorFrameReader*     m_pColorFrameReader;
	// Body reader
	IBodyFrameReader*      m_pBodyFrameReader;
	// Face sources
	IFaceFrameSource*	   m_pFaceFrameSources[BODY_COUNT];
	// Face readers
	IFaceFrameReader*	   m_pFaceFrameReaders[BODY_COUNT];
	// Direct2D
	RGBQUAD*               m_pColorRGBX;
};
