#include "CreateCardSetWidget.h"
#include "RussianApp.h"

CreateCardSetWidget::CreateCardSetWidget(const AccentedText& name,
	CardSetPackage::sptr package) :
	m_cardSetBrowser(package),
	m_labelName("Name:"),
	m_labelFileName("Filename:"),
	m_labelType("Card Set Type:"),
	m_inputName(name),
	m_buttonCreate("Create"),
	m_buttonCancel("Cancel")
{
	SetBackgroundColor(GUIConfig::color_background);

	m_inputType.SetText(EnumToShortString(CardSetType::k_other));

	m_layoutName.Add(&m_labelName);
	m_layoutName.Add(&m_inputName);
	m_layoutFileName.Add(&m_labelFileName);
	m_layoutFileName.Add(&m_inputFileName);
	m_layoutType.Add(&m_labelType);
	m_layoutType.Add(&m_inputType);
	m_layoutButtons.Add(&m_buttonCreate);
	m_layoutButtons.Add(&m_buttonCancel);
	m_mainLayout.Add(&m_layoutName);
	m_mainLayout.Add(&m_layoutFileName);
	m_mainLayout.Add(&m_layoutType);
	m_mainLayout.Add(&m_cardSetBrowser);
	m_mainLayout.Add(&m_layoutButtons);
	SetLayout(&m_mainLayout);

	// Connect signals
	m_buttonCreate.Clicked().Connect(this, &CreateCardSetWidget::OnClickCreate);
	m_buttonCancel.Clicked().Connect(this, &CreateCardSetWidget::OnClickCancel);
	m_inputName.TextEdited().Connect(this, &CreateCardSetWidget::OnEdited);
	m_inputType.TextEdited().Connect(this, &CreateCardSetWidget::OnEdited);
	m_inputFileName.TextEdited().Connect(this, &CreateCardSetWidget::OnEdited);

	UpdateState();
	Refresh();
}

void CreateCardSetWidget::OnInitialize()
{
	m_inputName.Focus();
}

void CreateCardSetWidget::UpdateState()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();

	m_package = m_cardSetBrowser.GetPackage();

	// Get and validate card info
	String typeText = ConvertToUTF8(m_inputType.GetText());
	m_cardSetType = CardSetType::k_other;
	m_validType = TryStringToEnum(typeText, m_cardSetType, true);
	m_name = AccentedText(m_inputName.GetText());
	m_key.name = ru::ToLower(m_name.GetString());

	CardSet::sptr cardSetFromKey = cardDatabase.GetCardSet(m_key);
	m_isKeyUnique = (cardSetFromKey == nullptr);

	m_validName = (m_isKeyUnique && !m_name.empty());

	// Auto-generate a file name from name
	unistr fileName = m_name.GetString();
	cmg::string::ReplaceAll(fileName, u" ", u"_");
	cmg::string::ReplaceAll(fileName, u"'", u" ");
	cmg::string::ReplaceAll(fileName, u"/", u"-");
	cmg::string::ReplaceAll(fileName, u"\\", u"-");
	cmg::string::ReplaceAll(fileName, u"<", u"-");
	cmg::string::ReplaceAll(fileName, u">", u"-");
	cmg::string::ReplaceAll(fileName, u"|", u"-");
	cmg::string::ReplaceAll(fileName, u"?", u"-");
	cmg::string::ReplaceAll(fileName, u"?", u"-");
	cmg::string::ReplaceAll(fileName, u":", u"-");
	cmg::string::ReplaceAll(fileName, u";", u"-");
	m_inputFileName.SetText(fileName);
	
	m_path = m_package->GetPath() / (fileName + u".json");
}

void CreateCardSetWidget::Refresh()
{
	// Name color
	if (!m_isKeyUnique)
		m_inputName.SetBackgroundColor(Config::k_colorEditedDuplicate);
	else if (!m_validName)
		m_inputName.SetBackgroundColor(Config::k_colorEditedInvalid);
	else
		m_inputName.SetBackgroundColor(GUIConfig::color_text_box_background);

	// CardSetType color
	if (!m_validType)
		m_inputType.SetBackgroundColor(Config::k_colorEditedInvalid);
	else
		m_inputType.SetBackgroundColor(GUIConfig::color_text_box_background);

	m_buttonCreate.SetEnabled(m_validName && m_validType);
}

void CreateCardSetWidget::OnClickCreate()
{
	auto& cardDatabase = GetApp()->GetCardDatabase();
	
	CardSet::sptr cardSet = cardDatabase.CreateCardSet(m_package, m_name,
		m_inputFileName.GetText(), m_cardSetType);

	if (cardSet)
	{
		m_cardSetCreated.Emit(cardSet);
		Close();
	}
}

void CreateCardSetWidget::OnClickCancel()
{
	Close();
}

void CreateCardSetWidget::OnEdited()
{
	UpdateState();
	Refresh();
}
