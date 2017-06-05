#pragma once
#include <QObject>

class MainView;
class InfoTab;
class Tomasulo;
class Ins;
class ViewModel;

class Startup final : public QObject
{
    Q_OBJECT
public:
    explicit Startup();
    void show() const;

    ~Startup();
private:
    InfoTab& m_infoTab;
    MainView& m_mainView;
    std::vector<Ins> &m_inss;
    Tomasulo& m_tomasulo;
    ViewModel& m_viewModel;

    explicit Startup(const Startup& rhs) = delete;
    Startup& operator= (const Startup& rhs) = delete;
};


