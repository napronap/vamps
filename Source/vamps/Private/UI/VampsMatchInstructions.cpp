#include "UI/VampsMatchInstructions.h"

#include "Input/Events.h"
#include "Input/NavigationReply.h"
#include "InputCoreTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

void SVampsMatchInstructions::Construct(const FArguments& InArgs)
{
	SetVisibility(EVisibility::SelfHitTestInvisible);
	const FSlateBrush* SolidBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
	const TSharedRef<SButton> Launcher = SAssignNew(OpenButton, SButton)
	                                         .ContentPadding(FMargin(18.f, 9.f))
	                                         .ButtonColorAndOpacity(FLinearColor(0.36f, 0.61f, 0.73f))
	                                         .OnClicked(this, &SVampsMatchInstructions::OpenFromInput);
	Launcher->SetContent(MakeText(LocalizedText(TEXT("遊び方"), TEXT("How to play")), "Bold", 15));

	const TSharedRef<SButton> Quit = SNew(SButton)
	                                     .ContentPadding(FMargin(18.f, 9.f))
	                                     .OnClicked_Lambda(
	                                         [QuitAction = InArgs._OnQuit]()
	                                         {
		                                         QuitAction.ExecuteIfBound();
		                                         return FReply::Handled();
	                                         });
	Quit->SetContent(MakeText(LocalizedText(TEXT("ゲームを終了"), TEXT("Quit Game")), "Bold", 15));

	const TSharedRef<SButton> LanguageToggle = SAssignNew(LanguageButton, SButton)
	                                               .ContentPadding(FMargin(16.f, 7.f))
	                                               .OnClicked(this, &SVampsMatchInstructions::ToggleLanguage);
	LanguageToggle->SetContent(MakeText(LocalizedText(TEXT("English"), TEXT("日本語")), "Bold", 14));

	const TSharedRef<SButton> Dismiss = SAssignNew(CloseButton, SButton)
	                                        .ContentPadding(FMargin(22.f, 8.f))
	                                        .OnClicked(this, &SVampsMatchInstructions::CloseFromInput);
	Dismiss->SetContent(MakeText(LocalizedText(TEXT("閉じる"), TEXT("Close")), "Bold", 15));

	const TSharedRef<SHorizontalBox> Header = SNew(SHorizontalBox);
	Header->AddSlot().FillWidth(1.f).VAlign(
	    VAlign_Center)[MakeText(LocalizedText(TEXT("遊び方"), TEXT("How to play")), "Bold", 26)];
	Header->AddSlot().AutoWidth()[LanguageToggle];

	const TSharedRef<SScrollBox> Scroll = SNew(SScrollBox);
	Scroll->AddSlot().Padding(0.f, 0.f, 10.f, 0.f)[MakeInstructions()];

	const TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);
	Panel->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 16.f)[Header];
	Panel->AddSlot().AutoHeight()[SNew(SBox).MaxDesiredHeight(470.f)[Scroll]];
	Panel->AddSlot().AutoHeight().HAlign(HAlign_Right)[Dismiss];

	const TSharedRef<SBorder> Sheet = SNew(SBorder)
	                                      .BorderImage(SolidBrush)
	                                      .BorderBackgroundColor(FLinearColor(0.035f, 0.050f, 0.068f, 1.f))
	                                      .Padding(FMargin(28.f, 22.f));
	Sheet->SetContent(SNew(SBox).WidthOverride(644.f)[Panel]);

	const FPointerEventHandler BlockClickThrough = FPointerEventHandler::CreateLambda(
	    [](const FGeometry&, const FPointerEvent&)
	    {
		    return FReply::Handled();
	    });
	const TSharedRef<SBorder> Backdrop = SAssignNew(ModalBackdrop, SBorder)
	                                         .Visibility(EVisibility::Collapsed)
	                                         .BorderImage(SolidBrush)
	                                         .BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.75f))
	                                         .OnMouseButtonDown(BlockClickThrough)
	                                         .OnMouseButtonUp(BlockClickThrough)
	                                         .OnMouseDoubleClick(BlockClickThrough)
	                                         .HAlign(HAlign_Center)
	                                         .VAlign(VAlign_Center);
	Backdrop->SetContent(Sheet);

	const TSharedRef<SHorizontalBox> Actions = SNew(SHorizontalBox);
	Actions->AddSlot().AutoWidth().Padding(0.f, 0.f, 8.f, 0.f)[Launcher];
	Actions->AddSlot().AutoWidth()[Quit];
	const TSharedRef<SOverlay> Layout = SNew(SOverlay).Visibility(EVisibility::SelfHitTestInvisible);
	Layout->AddSlot().HAlign(HAlign_Right).VAlign(VAlign_Top).Padding(24.f)[Actions];
	Layout->AddSlot()[Backdrop];
	ChildSlot[Layout];
}

TSharedRef<SWidget> SVampsMatchInstructions::MakeInstructions()
{
	const TSharedRef<SVerticalBox> Instructions = SNew(SVerticalBox);
	Instructions->AddSlot().AutoHeight().Padding(
	    0.f, 0.f, 0.f, 9.f)[MakeText(LocalizedText(TEXT("操作"), TEXT("Controls")), "Bold", 17)];
	Instructions->AddSlot().AutoHeight()[MakeControlRow(TEXT("WASD"), TEXT("移動"), TEXT("Move"))];
	Instructions->AddSlot().AutoHeight()[MakeControlRow(
	    TEXT("RMB"), TEXT("右クリックで攻撃・ガード"), TEXT("Right-click to attack or guard"))];
	Instructions->AddSlot()
	    .AutoHeight()[MakeControlRow(TEXT("F"), TEXT("ロックオン対象を切り替え"), TEXT("Switch targets"))];
	Instructions->AddSlot().AutoHeight()[MakeControlRow(TEXT("Esc"), TEXT("ポーズ"), TEXT("Pause"))];
	Instructions->AddSlot()
	    .AutoHeight()[MakeControlRow(TEXT("Left Shift"),
	                                 TEXT("押すとダッシュ。方向入力がなければ後退。移動しながら長押しでスプリント"),
	                                 TEXT("Tap to dash or backstep with no direction. Hold while moving to sprint."))];
	Instructions->AddSlot().AutoHeight().Padding(
	    0.f, 16.f, 0.f, 9.f)[MakeText(LocalizedText(TEXT("対戦ルール"), TEXT("Match rules")), "Bold", 17)];

	const TCHAR* JapaneseRules = TEXT(
	    "敵を倒して得点。先に目標点に達するか、時間切れの時点で得点が多いチームが勝ちます。\n\n"
	    "攻撃がぶつかると、先に始めた攻撃が優先されます。"
	    "自分の攻撃が当たるかガードされたら、受付時間内に右クリックするとコンボが続きます。\n\n"
	    "相手の連続攻撃を受けたら、右クリックを連打しましょう。一撃をガードするたびに、次の入力が必要です。\n\n"
	    "ダッシュとスプリントはスタミナを消費します。使わずにいると回復します。ガードするたびにゲージがたまり、満"
	    "タンになるとスタンします。\n\n"
	    "攻撃を出し切った直後は、少しの間だけ攻撃もガードもできません。ダッシュやスプリントで距離を取りましょう。");
	const TCHAR* EnglishRules = TEXT(
	    "Defeat opponents to score. Reach the score goal first, or have more points when time runs out.\n\n"
	    "When active attacks clash, the attack that started first has priority. "
	    "If your attack lands or is blocked, tap RMB during the combo window to chain.\n\n"
	    "Mash RMB while you're under attack. Each block needs a fresh press.\n\n"
	    "Dashing and sprinting spend stamina. It recovers after a short break. Blocking fills the guard gauge. A full "
	    "gauge stuns you.\n\n"
	    "After your attack ends, you briefly can't attack or block. Dash or sprint away to make space while you "
	    "recover.");
	const TSharedRef<STextBlock> RulesText = MakeText(LocalizedText(JapaneseRules, EnglishRules), "Regular", 14);
	RulesText->SetAutoWrapText(true);
	RulesText->SetLineHeightPercentage(1.18f);
	Instructions->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 16.f)[RulesText];
	return Instructions;
}

TSharedRef<SWidget> SVampsMatchInstructions::MakeControlRow(const TCHAR* Key,
                                                            const TCHAR* Japanese,
                                                            const TCHAR* English)
{
	const TSharedRef<STextBlock> KeyText = MakeText(FText::FromString(Key), "Bold", 14);
	KeyText->SetColorAndOpacity(FLinearColor(0.45f, 0.83f, 0.94f));

	const TSharedRef<STextBlock> ActionText = MakeText(LocalizedText(Japanese, English), "Regular", 14);
	ActionText->SetAutoWrapText(true);

	const TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox);
	Row->AddSlot().AutoWidth().Padding(0.f, 3.f, 10.f, 3.f)[SNew(SBox).WidthOverride(90.f)[KeyText]];
	Row->AddSlot().FillWidth(1.f).Padding(0.f, 3.f, 0.f, 3.f)[ActionText];
	return Row;
}

TSharedRef<STextBlock> SVampsMatchInstructions::MakeText(TAttribute<FText> Text, const FName& Weight, int32 Size)
{
	return SNew(STextBlock)
	    .Text(MoveTemp(Text))
	    .Font(FCoreStyle::GetDefaultFontStyle(Weight, Size))
	    .ColorAndOpacity(FLinearColor(0.92f, 0.94f, 0.96f));
}

TAttribute<FText> SVampsMatchInstructions::LocalizedText(const TCHAR* Japanese, const TCHAR* English)
{
	return TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(
	    this, &SVampsMatchInstructions::SelectLanguage, FText::FromString(Japanese), FText::FromString(English)));
}

FText SVampsMatchInstructions::SelectLanguage(FText Japanese, FText English) const
{
	return bEnglish ? English : Japanese;
}

FReply SVampsMatchInstructions::OpenFromInput()
{
	ModalBackdrop->SetVisibility(EVisibility::Visible);
	return FReply::Handled().SetUserFocus(CloseButton.ToSharedRef(), EFocusCause::SetDirectly);
}

void SVampsMatchInstructions::Close()
{
	ModalBackdrop->SetVisibility(EVisibility::Collapsed);
}

FReply SVampsMatchInstructions::CloseFromInput()
{
	Close();
	return FReply::Handled().SetUserFocus(OpenButton.ToSharedRef(), EFocusCause::SetDirectly);
}

FReply SVampsMatchInstructions::ToggleLanguage()
{
	bEnglish = !bEnglish;
	return FReply::Handled();
}

FReply SVampsMatchInstructions::OnPreviewKeyDown(const FGeometry&, const FKeyEvent& InKeyEvent)
{
	return ModalBackdrop->GetVisibility().IsVisible() && InKeyEvent.GetKey() == EKeys::Escape ? CloseFromInput()
	                                                                                          : FReply::Unhandled();
}

FNavigationReply SVampsMatchInstructions::OnNavigation(const FGeometry& MyGeometry,
                                                       const FNavigationEvent& InNavigationEvent)
{
	if (ModalBackdrop->GetVisibility().IsVisible())
	{
		return FNavigationReply::Explicit(CloseButton->HasAnyUserFocus() ? LanguageButton : CloseButton);
	}
	return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
}
