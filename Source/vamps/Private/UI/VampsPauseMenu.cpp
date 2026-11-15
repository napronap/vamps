#include "UI/VampsPauseMenu.h"

#include "Input/Events.h"
#include "Input/NavigationReply.h"
#include "InputCoreTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SVampsPauseMenu::Construct(const FArguments& InArgs)
{
	ResumeAction = InArgs._OnResume;
	QuitAction = InArgs._OnQuit;

	const FSlateBrush* SolidBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
	const FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Bold", 15);
	const FLinearColor TextColor(0.92f, 0.94f, 0.96f);
	const FMargin ButtonPadding(18.f, 9.f);
	const TSharedRef<SButton> Resume = SAssignNew(ResumeButton, SButton)
	                                       .HAlign(HAlign_Center)
	                                       .ContentPadding(ButtonPadding)
	                                       .ButtonColorAndOpacity(FLinearColor(0.36f, 0.61f, 0.73f))
	                                       .OnClicked(this, &SVampsPauseMenu::ResumeFromInput);
	Resume->SetContent(SNew(STextBlock)
	                       .Text(NSLOCTEXT("VampsPauseMenu", "Resume", "Resume"))
	                       .Font(ButtonFont)
	                       .ColorAndOpacity(TextColor));

	const TSharedRef<SButton> Quit = SAssignNew(QuitButton, SButton)
	                                     .HAlign(HAlign_Center)
	                                     .ContentPadding(ButtonPadding)
	                                     .OnClicked(this, &SVampsPauseMenu::QuitFromInput);
	Quit->SetContent(SNew(STextBlock)
	                     .Text(NSLOCTEXT("VampsPauseMenu", "QuitGame", "Quit Game"))
	                     .Font(ButtonFont)
	                     .ColorAndOpacity(TextColor));

	const TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);
	Panel->AddSlot()
	    .AutoHeight()
	    .HAlign(HAlign_Center)
	    .Padding(0.f, 0.f, 0.f, 20.f)[SNew(STextBlock)
	                                      .Text(NSLOCTEXT("VampsPauseMenu", "Paused", "Paused"))
	                                      .Font(FCoreStyle::GetDefaultFontStyle("Bold", 26))
	                                      .ColorAndOpacity(TextColor)];
	Panel->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)[Resume];
	Panel->AddSlot().AutoHeight()[Quit];

	const TSharedRef<SBorder> Sheet = SNew(SBorder)
	                                      .BorderImage(SolidBrush)
	                                      .BorderBackgroundColor(FLinearColor(0.035f, 0.050f, 0.068f, 1.f))
	                                      .Padding(FMargin(28.f, 22.f));
	Sheet->SetContent(SNew(SBox).WidthOverride(260.f)[Panel]);

	const FPointerEventHandler BlockClickThrough = FPointerEventHandler::CreateLambda(
	    [](const FGeometry&, const FPointerEvent&)
	    {
		    return FReply::Handled();
	    });
	const TSharedRef<SBorder> Backdrop = SNew(SBorder)
	                                         .BorderImage(SolidBrush)
	                                         .BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.75f))
	                                         .Padding(0.f)
	                                         .OnMouseButtonDown(BlockClickThrough)
	                                         .OnMouseButtonUp(BlockClickThrough)
	                                         .OnMouseDoubleClick(BlockClickThrough)
	                                         .HAlign(HAlign_Center)
	                                         .VAlign(VAlign_Center);
	Backdrop->SetContent(Sheet);
	ChildSlot[Backdrop];
}

TSharedRef<SWidget> SVampsPauseMenu::GetResumeButton() const
{
	return ResumeButton.ToSharedRef();
}

FReply SVampsPauseMenu::OnPreviewKeyDown(const FGeometry&, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		return InKeyEvent.IsRepeat() ? FReply::Handled() : ResumeFromInput();
	}
	return FReply::Unhandled();
}

FNavigationReply SVampsPauseMenu::OnNavigation(const FGeometry&, const FNavigationEvent& InNavigationEvent)
{
	const EUINavigation Direction = InNavigationEvent.GetNavigationType();
	if (Direction == EUINavigation::Left || Direction == EUINavigation::Right)
	{
		return FNavigationReply::Stop();
	}

	const bool bResumeFocused = ResumeButton->HasUserFocus(InNavigationEvent.GetUserIndex()).IsSet();
	return FNavigationReply::Explicit(bResumeFocused ? QuitButton : ResumeButton);
}

FReply SVampsPauseMenu::ResumeFromInput()
{
	ResumeAction.ExecuteIfBound();
	return FReply::Handled();
}

FReply SVampsPauseMenu::QuitFromInput()
{
	QuitAction.ExecuteIfBound();
	return FReply::Handled();
}
