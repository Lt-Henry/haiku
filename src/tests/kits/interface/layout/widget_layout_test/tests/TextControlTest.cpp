/*
 * Copyright 2008, Stephan Aßmus <superstippi@gmx.de>.
 * Copyright 2007, Ingo Weinhold <bonefish@cs.tu-berlin.de>.
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "TextControlTest.h"

#include <stdio.h>

#include <TextControl.h>

#include "CheckBox.h"
#include "GroupView.h"


enum {
	MSG_CHANGE_LABEL_TEXT = 'chlt',
	MSG_CHANGE_LABEL_FONT = 'chlf',
	MSG_CHANGE_INVALID = 'chiv'
};


// constructor
TextControlTest::TextControlTest()
	:
	ControlTest("TextControl"),
	fLongTextCheckBox(NULL),
	fBigFontCheckBox(NULL),
	fInvalidCheckBox(NULL),
	fDefaultFont(NULL),
	fBigFont(NULL)
{
	fTextControl = new BTextControl("Label", "Some Text", NULL);

	SetView(fTextControl);
}


// destructor
TextControlTest::~TextControlTest()
{
	delete fDefaultFont;
	delete fBigFont;
}


// CreateTest
Test*
TextControlTest::CreateTest()
{
	return new TextControlTest;
}


// ActivateTest
void
TextControlTest::ActivateTest(View* controls)
{
	GroupView* group = new GroupView(B_VERTICAL);
	group->SetFrame(controls->Bounds());
	group->SetSpacing(0, 8);
	controls->AddChild(group);

	// BMenuField sets its background color to that of its parent in
	// AttachedToWindow(). Override.
	fTextControl->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	fTextControl->SetLowUIColor(B_PANEL_BACKGROUND_COLOR);

	// long text
	fLongTextCheckBox = new LabeledCheckBox("Long label text",
		new BMessage(MSG_CHANGE_LABEL_TEXT), this);
	group->AddChild(fLongTextCheckBox);

	// big font
	fBigFontCheckBox = new LabeledCheckBox("Big label font",
		new BMessage(MSG_CHANGE_LABEL_FONT), this);
	group->AddChild(fBigFontCheckBox);

	fInvalidCheckBox = new LabeledCheckBox("Invalid Input", new BMessage(MSG_CHANGE_INVALID), this);
	group->AddChild(fInvalidCheckBox);

	_UpdateLabelText();
	_UpdateLabelFont();

	group->AddChild(new Glue());
}


// DectivateTest
void
TextControlTest::DectivateTest()
{
}


// MessageReceived
void
TextControlTest::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_CHANGE_LABEL_TEXT:
			_UpdateLabelText();
			break;
		case MSG_CHANGE_LABEL_FONT:
			_UpdateLabelFont();
			break;
		case MSG_CHANGE_INVALID:
			_UpdateInvalid();
			break;
		default:
			Test::MessageReceived(message);
			break;
	}
}


void
TextControlTest::_UpdateInvalid()
{
	if (!fInvalidCheckBox)
		return;

	if (fInvalidCheckBox->IsSelected())
		fTextControl->MarkAsInvalid(true);
	else
		fTextControl->MarkAsInvalid(false);
}


// _UpdateLabelText
void
TextControlTest::_UpdateLabelText()
{
	if (!fLongTextCheckBox || !fTextControl)
		return;

	fTextControl->SetLabel(fLongTextCheckBox->IsSelected()
		? "Pretty long text control label"
		: "Short label");
}


// _UpdateLabelFont
void
TextControlTest::_UpdateLabelFont()
{
	if (!fBigFontCheckBox || !fTextControl || !fTextControl->Window())
		return;

	// get default font lazily
	if (!fDefaultFont) {
		fDefaultFont = new BFont;
		fTextControl->GetFont(fDefaultFont);

		fBigFont = new BFont(fDefaultFont);
		fBigFont->SetSize(20);
	}

	// set font
	fTextControl->SetFont(fBigFontCheckBox->IsSelected()
		? fBigFont : fDefaultFont);
	fTextControl->InvalidateLayout();
	fTextControl->Invalidate();
}
