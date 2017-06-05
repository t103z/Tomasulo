#include "startup.h"
#include "View/infotab.h"
#include "View/mainview.h"
#include "Model/Tomasulo.h"
#include "Model/Tomasulo/Ins.h"
#include "ViewModel/viewmodel.h"

Startup::Startup() :
    QObject(nullptr),
    m_infoTab(*new InfoTab(nullptr)),
    m_mainView(*new MainView(nullptr, m_infoTab)),
    m_inss(*new std::vector<Ins>),
    m_tomasulo(*new Tomasulo(m_inss)),
    m_viewModel(*new ViewModel(m_mainView, m_infoTab, m_inss, m_tomasulo))
{
}

void Startup::show() const {
    m_mainView.show();
}

Startup::~Startup() {
    delete &m_mainView;
    delete &m_inss;
    delete &m_tomasulo;
    delete &m_viewModel;
}
