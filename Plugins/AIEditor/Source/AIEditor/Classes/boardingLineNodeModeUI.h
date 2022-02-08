// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "boardingLineNode.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "ManagerModeUI.h"
#include "boardingLineNodeUI.h"
#include "CoreMinimal.h"

/**
 * 
 */
class AIEDITOR_API boardingLineNodeModeUI
{
public:
	boardingLineNodeModeUI();
	~boardingLineNodeModeUI();
	static TSharedPtr<SSlider>						  BoardingLineRadiusSlider;
	static TSharedPtr<STextBlock>					  BoardingLineRadiusLabel;
	static AboardingLineNode* currentBoardingLineNode;
	static EVisibility boardingLineNodeModeToggle();
	static void DrawDirectionLine(AboardingLineNode* boardingNode = nullptr);
	static FReply DeleteboardingLineNode();
 
	static FReply exitBoardingMode();
 
	static void updateAngle(float NewValue);
	static TSharedRef<SWidget> makeBoardingLineNodeModeUI();
 };
