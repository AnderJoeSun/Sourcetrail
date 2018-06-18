#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "qt/utility/QtFlowLayout.h"
#include "settings/LanguageType.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"

QtProjectWizzardContentSelect::QtProjectWizzardContentSelect(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
{
}

void QtProjectWizzardContentSelect::populate(QGridLayout* layout, int& row)
{
	struct SourceGroupInfo
	{
		SourceGroupInfo(SourceGroupType type) :type(type) {}
		const SourceGroupType type;
	};

	// define which kind of source groups are available for each language
	std::map<LanguageType, std::vector<SourceGroupInfo>> sourceGroupInfos;
	sourceGroupInfos[LANGUAGE_C].push_back(SourceGroupInfo(SOURCE_GROUP_C_EMPTY));
	sourceGroupInfos[LANGUAGE_C].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_CDB));
	sourceGroupInfos[LANGUAGE_C].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_VS));
	sourceGroupInfos[LANGUAGE_C].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_CODEBLOCKS));
	// sourceGroupInfos[LANGUAGE_C].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_SONARGRAPH));
	sourceGroupInfos[LANGUAGE_CPP].push_back(SourceGroupInfo(SOURCE_GROUP_CPP_EMPTY));
	sourceGroupInfos[LANGUAGE_CPP].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_CDB));
	sourceGroupInfos[LANGUAGE_CPP].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_VS));
	sourceGroupInfos[LANGUAGE_CPP].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_CODEBLOCKS));
	// sourceGroupInfos[LANGUAGE_CPP].push_back(SourceGroupInfo(SOURCE_GROUP_CXX_SONARGRAPH));
	sourceGroupInfos[LANGUAGE_JAVA].push_back(SourceGroupInfo(SOURCE_GROUP_JAVA_EMPTY));
	sourceGroupInfos[LANGUAGE_JAVA].push_back(SourceGroupInfo(SOURCE_GROUP_JAVA_MAVEN));
	sourceGroupInfos[LANGUAGE_JAVA].push_back(SourceGroupInfo(SOURCE_GROUP_JAVA_GRADLE));
	// sourceGroupInfos[LANGUAGE_JAVA].push_back(SourceGroupInfo(SOURCE_GROUP_JAVA_SONARGRAPH));

	// define which icons should be used for which kind of source group
	m_sourceGroupTypeIconName[SOURCE_GROUP_C_EMPTY] = L"empty_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_CPP_EMPTY] = L"empty_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_CXX_CDB] = L"cdb_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_CXX_VS] = L"vs_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_CXX_SONARGRAPH] = L"sonargraph_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_CXX_CODEBLOCKS] = L"cbp_icon"; // TODO: change this
	m_sourceGroupTypeIconName[SOURCE_GROUP_JAVA_EMPTY] = L"empty_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_JAVA_MAVEN] = L"mvn_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_JAVA_GRADLE] = L"gradle_icon";
	m_sourceGroupTypeIconName[SOURCE_GROUP_JAVA_SONARGRAPH] = L"sonargraph_icon";

	// define descriptions for each kind of Source Group
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_C_EMPTY] = "Create a new Sourcetrail Source Group by defining which C files will be indexed.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_CPP_EMPTY] = "Create a new Sourcetrail Source Group by defining which C++ files will be indexed.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_CXX_CDB] =
		"Create a Source Group from an existing Compilation Database (compile_commands.json). "
		"They can be exported from CMake<br />(-D CMAKE_EXPORT_COMPILE_COMMANDS=1) and Make projects. Have a look at the "
		"<a href=\"https://sourcetrail.com/documentation/#CreateAProjectFromCompilationDatabase\">"
		"documentation</a>.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_CXX_VS] =
		"<p>Create a new Source Group from an existing Visual Studio Solution file.</p>"
		"<p><b>Note</b>: Requires a running Visual Studio instance with the "
		"<a href=\"https://sourcetrail.com/documentation/index.html#VisualStudio\">Sourcetrail Visual Studio Extension</a> installed.</p>";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_CXX_SONARGRAPH] = "Create a new Source Group from an existing <a href=\"https://www.hello2morrow.com/products/sonargraph\">Sonargraph</a> project file.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_CXX_CODEBLOCKS] =
		"<p>Create a new Source Group from an existing Code::Blocks project file.</p>"
		"<p><b>Note</b>: A \".cbp\" file will also be generated by the <b>Qt Creator</b> if a CMakeLists file is imported.</p>";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_JAVA_EMPTY] = "Create a new Sourcetrail Source Group by defining which Java files will be indexed.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_JAVA_MAVEN] = "Create a new Source Group from an existing Maven project.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_JAVA_GRADLE] = "Create a new Source Group from an existing Gradle project.";
	m_sourceGroupTypeDescriptions[SOURCE_GROUP_JAVA_SONARGRAPH] = "Create a new Source Group from an existing Sonargraph project file.";

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 30, 0, 0);
	vlayout->setSpacing(10);

	m_languages = new QButtonGroup();
	for (auto& it: sourceGroupInfos)
	{
		QPushButton* b = new QPushButton(languageTypeToString(it.first).c_str(), this);
		b->setObjectName("menuButton");
		b->setCheckable(true);
		b->setProperty("language_type", it.first);
		m_languages->addButton(b);
		vlayout->addWidget(b);
	}

	vlayout->addStretch();
	layout->addLayout(vlayout, 0, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	connect(m_languages, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
		[this](QAbstractButton* button)
		{
			LanguageType selectedLanguage = LANGUAGE_UNKNOWN;
			bool ok = false;
			int languageTypeInt = button->property("language_type").toInt(&ok);
			if (ok)
			{
				selectedLanguage = LanguageType(languageTypeInt);
			}

			for (auto& it: m_buttons)
			{
				it.second->setExclusive(false);
				for (QAbstractButton* button : it.second->buttons())
				{
					button->setChecked(false);
					button->setVisible(it.first == selectedLanguage);
				}
				it.second->setExclusive(true);
			}

			m_window->setNextEnabled(false);
			m_title->setText("Source Group Types - " + m_languages->checkedButton()->text());
			m_description->setText("");
		}
	);

	QtFlowLayout* flayout = new QtFlowLayout(10, 0, 0);

	for (auto& languageIt: sourceGroupInfos)
	{
		QButtonGroup* sourceGroupButtons = new QButtonGroup(this);

		for (auto& sourceGroupIt: languageIt.second)
		{
			QToolButton* b = createSourceGroupButton(
				utility::insertLineBreaksAtBlankSpaces(sourceGroupTypeToProjectSetupString(sourceGroupIt.type), 15).c_str(),
				QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/" + m_sourceGroupTypeIconName[sourceGroupIt.type] + L".png").wstr())
			);
			b->setProperty("source_group_type", int(sourceGroupIt.type));
			sourceGroupButtons->addButton(b);
			flayout->addWidget(b);
		}

		m_buttons[languageIt.first] = sourceGroupButtons;
	}

	for (auto& it: m_buttons)
	{
		connect(it.second, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
			[this](QAbstractButton* button)
			{
				SourceGroupType selectedType = SOURCE_GROUP_UNKNOWN;
				bool ok = false;
				int selectedTypeInt = button->property("source_group_type").toInt(&ok);
				if (ok)
				{
					selectedType = SourceGroupType(selectedTypeInt);
				}

				m_description->setText(m_sourceGroupTypeDescriptions[selectedType].c_str());

				m_window->setNextEnabled(true);
			}
		);
	}

	QWidget* container = new QWidget();
	QVBoxLayout* containerLayout = new QVBoxLayout();
	containerLayout->setContentsMargins(0, 0, 0, 0);

	QFrame* groupContainer = new QFrame();
	groupContainer->setObjectName("sourceGroupContainer");
	groupContainer->setLayout(flayout);
	containerLayout->addWidget(groupContainer, 0);

	m_description = new QLabel(" \n \n");
	m_description->setWordWrap(true);
	m_description->setOpenExternalLinks(true);
	m_description->setObjectName("sourceGroupDescription");
	m_description->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
	m_description->setMinimumHeight(80);
	containerLayout->addWidget(m_description, 0);

	container->setLayout(containerLayout);
	layout->addWidget(container, 0, QtProjectWizzardWindow::BACK_COL);

	m_title = new QLabel("Source Group Types");
	m_title->setObjectName("sourceGroupTitle");

	layout->addWidget(m_title, 0, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);

	layout->setRowStretch(0, 0);
	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 0);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 1);
	layout->setHorizontalSpacing(0);

	m_languages->buttons().constFirst()->click();
}

void QtProjectWizzardContentSelect::save()
{
	SourceGroupType selectedType;

	for (auto& it: m_buttons)
	{
		if (QAbstractButton* b = it.second->checkedButton())
		{
			bool ok = false;
			int selectedTypeInt = b->property("source_group_type").toInt(&ok);
			if (ok)
			{
				selectedType = SourceGroupType(selectedTypeInt);
				break;
			}
		}
	}
	emit selected(selectedType);
}

bool QtProjectWizzardContentSelect::check()
{
	bool sourceGroupChosen = false;

	for (auto& it: m_buttons)
	{
		if (it.second->checkedId() != -1)
		{
			sourceGroupChosen = true;
			break;
		}
	}

	if (!sourceGroupChosen)
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose a method of creating a new Source Group.");
		msgBox.exec();
		return false;
	}

	return true;
}

bool QtProjectWizzardContentSelect::isScrollAble() const
{
	return true;
}
