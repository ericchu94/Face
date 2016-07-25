// Fill out your copyright notice in the Description page of Project Settings.

#include "Face.h"
#include "MyActor.h"
#include "AllowWindowsPlatformTypes.h"
#include <Kinect.h>
#include <Kinect.Face.h>
#include "HideWindowsPlatformTypes.h"
#include <iostream>

// face property text layout offset in X axis
static const float c_FaceTextLayoutOffsetX = -0.1f;

// face property text layout offset in Y axis
static const float c_FaceTextLayoutOffsetY = -0.125f;

// define the face frame features required to be computed by this application
static const unsigned long c_FaceFrameFeatures =
FaceFrameFeatures::FaceFrameFeatures_BoundingBoxInColorSpace
| FaceFrameFeatures::FaceFrameFeatures_PointsInColorSpace
| FaceFrameFeatures::FaceFrameFeatures_RotationOrientation
| FaceFrameFeatures::FaceFrameFeatures_Happy
| FaceFrameFeatures::FaceFrameFeatures_RightEyeClosed
| FaceFrameFeatures::FaceFrameFeatures_LeftEyeClosed
| FaceFrameFeatures::FaceFrameFeatures_MouthOpen
| FaceFrameFeatures::FaceFrameFeatures_MouthMoved
| FaceFrameFeatures::FaceFrameFeatures_LookingAway
| FaceFrameFeatures::FaceFrameFeatures_Glasses
| FaceFrameFeatures::FaceFrameFeatures_FaceEngagement;


// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	InitializeDefaultSensor();
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AMyActor::GetMouthLeft(float DeltaSeconds)
{
	return GetMouth(DeltaSeconds);
}

float AMyActor::GetMouthRight(float DeltaSeconds)
{
	return GetMouth(DeltaSeconds);
}

float AMyActor::GetMouth(float DeltaSeconds)
{
	Update();
	/*RunningTime += DeltaSeconds;
	float IntPart;
	float FractPart = FMath::Modf(RunningTime, &IntPart);

	if (FMath::Fmod(IntPart, 2) < 1)
		return FractPart;*/

	return happyDetected;
}

bool AMyActor::IsKinectReady() {
	InitializeDefaultSensor();
	return KinectReady;
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>S_OK on success else the failure code</returns>
HRESULT AMyActor::InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize Kinect and get color, body and face readers
		IColorFrameSource* pColorFrameSource = nullptr;
		IBodyFrameSource* pBodyFrameSource = nullptr;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		if (SUCCEEDED(hr))
		{
			// create a face frame source + reader to track each body in the fov
			for (int i = 0; i < BODY_COUNT; i++)
			{
				if (SUCCEEDED(hr))
				{
					// create the face frame source by specifying the required face frame features
					hr = CreateFaceFrameSource(m_pKinectSensor, 0, c_FaceFrameFeatures, &m_pFaceFrameSources[i]);
				}
				if (SUCCEEDED(hr))
				{
					// open the corresponding reader
					hr = m_pFaceFrameSources[i]->OpenReader(&m_pFaceFrameReaders[i]);
				}
			}
		}

		SafeRelease(pColorFrameSource);
		SafeRelease(pBodyFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		KinectReady = true;
		return E_FAIL;
	}
	KinectReady = false;
	return hr;
}

/// <summary>
/// Main processing function
/// </summary>
void AMyActor::Update()
{
	if (!m_pColorFrameReader || !m_pBodyFrameReader)
	{
		return;
	}

	IColorFrame* pColorFrame = nullptr;
	HRESULT hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = nullptr;
		int nWidth = 0;
		int nHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		unsigned int nBufferSize = 0;
		RGBQUAD *pBuffer = nullptr;

		hr = pColorFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		/*if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
			}
			else if (m_pColorRGBX)
			{
				pBuffer = m_pColorRGBX;
				nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}*/

		if (SUCCEEDED(hr))
		{
			ProcessFaces();
		}

		SafeRelease(pFrameDescription);
	}

	SafeRelease(pColorFrame);
}

/// <summary>
/// Processes new face frames
/// </summary>
void AMyActor::ProcessFaces()
{
	HRESULT hr;
	IBody* ppBodies[BODY_COUNT] = { 0 };
	bool bHaveBodyData = SUCCEEDED(UpdateBodyData(ppBodies));

	// iterate through each face reader
	for (int iFace = 0; iFace < BODY_COUNT; ++iFace)
	{
		// retrieve the latest face frame from this reader
		IFaceFrame* pFaceFrame = nullptr;
		hr = m_pFaceFrameReaders[iFace]->AcquireLatestFrame(&pFaceFrame);

		BOOLEAN bFaceTracked = false;
		if (SUCCEEDED(hr) && nullptr != pFaceFrame)
		{
			// check if a valid face is tracked in this face frame
			hr = pFaceFrame->get_IsTrackingIdValid(&bFaceTracked);
		}

		if (SUCCEEDED(hr))
		{
			if (bFaceTracked)
			{
				IFaceFrameResult* pFaceFrameResult = nullptr;
				RectI faceBox = { 0 };
				PointF facePoints[FacePointType::FacePointType_Count];
				Vector4 faceRotation;
				DetectionResult faceProperties[FaceProperty::FaceProperty_Count];

				hr = pFaceFrame->get_FaceFrameResult(&pFaceFrameResult);

				// need to verify if pFaceFrameResult contains data before trying to access it
				if (SUCCEEDED(hr) && pFaceFrameResult != nullptr)
				{
					hr = pFaceFrameResult->get_FaceBoundingBoxInColorSpace(&faceBox);

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->GetFacePointsInColorSpace(FacePointType::FacePointType_Count, facePoints);
					}

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->get_FaceRotationQuaternion(&faceRotation);
					}

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->GetFaceProperties(FaceProperty::FaceProperty_Count, faceProperties);
					}

					if (faceProperties[FaceProperty::FaceProperty_Happy] == DetectionResult::DetectionResult_Yes) {
						happyDetected = true;
					}
					else {
						happyDetected = false;
					}
				}

				SafeRelease(pFaceFrameResult);
			}
			else
			{
				// face tracking is not valid - attempt to fix the issue
				// a valid body is required to perform this step
				if (bHaveBodyData)
				{
					// check if the corresponding body is tracked 
					// if this is true then update the face frame source to track this body
					IBody* pBody = ppBodies[iFace];
					if (pBody != nullptr)
					{
						BOOLEAN bTracked = false;
						hr = pBody->get_IsTracked(&bTracked);

						UINT64 bodyTId;
						if (SUCCEEDED(hr) && bTracked)
						{
							// get the tracking ID of this body
							hr = pBody->get_TrackingId(&bodyTId);
							if (SUCCEEDED(hr))
							{
								// update the face frame source with the tracking ID
								m_pFaceFrameSources[iFace]->put_TrackingId(bodyTId);
							}
						}
					}
				}
			}
		}

		SafeRelease(pFaceFrame);
	}

	if (bHaveBodyData)
	{
		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}
}

/// <summary>
/// Updates body data
/// </summary>
/// <param name="ppBodies">pointer to the body data storage</param>
/// <returns>indicates success or failure</returns>
HRESULT AMyActor::UpdateBodyData(IBody** ppBodies)
{
	HRESULT hr = E_FAIL;

	if (m_pBodyFrameReader != nullptr)
	{
		IBodyFrame* pBodyFrame = nullptr;
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
		}
		SafeRelease(pBodyFrame);
	}

	return hr;
}

