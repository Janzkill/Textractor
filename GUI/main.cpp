#include "mainwindow.h"
#include "module.h"
#include <winhttp.h>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

extern const wchar_t* UPDATE_AVAILABLE;
static void applyDarkTheme(QApplication& app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    const QColor base(30, 30, 30), window(45, 45, 45), text(220, 220, 220);
    const QColor disabled(127, 127, 127), accent(42, 130, 218);

    p.setColor(QPalette::Window, window);
    p.setColor(QPalette::WindowText, text);
    p.setColor(QPalette::Base, base);
    p.setColor(QPalette::AlternateBase, window);
    p.setColor(QPalette::Text, text);
    p.setColor(QPalette::Button, window);
    p.setColor(QPalette::ButtonText, text);
    p.setColor(QPalette::ToolTipBase, base);
    p.setColor(QPalette::ToolTipText, text);
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, accent);
    p.setColor(QPalette::Highlight, accent);
    p.setColor(QPalette::HighlightedText, Qt::black);
    p.setColor(QPalette::Disabled, QPalette::Text, disabled);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    p.setColor(QPalette::Disabled, QPalette::WindowText, disabled);

    app.setPalette(p);
}

int main(int argc, char *argv[])
{
	std::thread([]
	{
		if (!*VERSION) return;
		using InternetHandle = AutoHandle<Functor<WinHttpCloseHandle>>;
		// Queries GitHub releases API https://developer.github.com/v3/repos/releases/ and checks the last release tag to check if it's the same
		if (InternetHandle internet = WinHttpOpen(L"Mozilla/5.0 Textractor", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0))
			if (InternetHandle connection = WinHttpConnect(internet, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0))
				if (InternetHandle request = WinHttpOpenRequest(connection, L"GET", L"/repos/Artikash/Textractor/releases", NULL, NULL, NULL, WINHTTP_FLAG_SECURE))
					if (WinHttpSendRequest(request, NULL, 0, NULL, 0, 0, NULL))
					{
						char buffer[1000] = {};
						WinHttpReceiveResponse(request, NULL);
						WinHttpReadData(request, buffer, 1000, DUMMY);
						if (abs(strstr(buffer, "/tag/") - strstr(buffer, VERSION)) > 10) TEXTRACTOR_MESSAGE(UPDATE_AVAILABLE);
					}
	}).detach();

	QDir::setCurrent(QFileInfo(S(GetModuleFilename().value())).absolutePath());

	QApplication app(argc, argv);
	applyDarkTheme(app);
	app.setFont(QFont("MS Shell Dlg 2", 10));
	return MainWindow().show(), app.exec();
}
