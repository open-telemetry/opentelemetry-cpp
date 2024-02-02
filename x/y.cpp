#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <cstdio>
#include <thread>
#include <atomic>
#include <deque>
#include <vector>

#include <windows.h>

void BindStdHandlesToConsole()
{
#ifdef _WIN32
    FreeConsole();
	  AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);
    HANDLE hStdout = CreateFileA("CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hStdin = CreateFileA("CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE,hStdout);
    SetStdHandle(STD_ERROR_HANDLE,hStdout);
    SetStdHandle(STD_INPUT_HANDLE,hStdin);
    std::wclog.clear();
    std::clog.clear();
    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
    std::cerr.clear();
    std::cout.clear();
    std::cin.clear();
#endif
}

std::vector<std::string> textLines()
{
  std::vector<std::string> lines;
  const auto stream{ _popen("dir /s f:\\p\\malkia\\ftxui", "r") };
  char buf[1024]{};
  for( ;; )
  {
    fgets(buf,sizeof(buf),stream);
    if(!buf[0])
      break;
    char* p = strchr(buf,'\n'); if(p) *p=0;
    p = strchr(buf,'\r'); if(p) *p=0;
    lines.emplace_back(buf);
    if( lines.size() > 500 )
      break;
  }
  return lines;
}

int main() {
  BindStdHandlesToConsole();
  using namespace ftxui;

  //auto screen = ScreenInteractive::Fullscreen(); //TerminalOutput();
  //auto screen = ScreenInteractive::TerminalOutput();
  auto screen = ScreenInteractive::FitComponent();

  std::atomic<bool> refreshing = true;
  auto lines = textLines();
  /*
  std::thread refresh([&refreshing, &screen, &lines]{
    FILE* stream=nullptr;
    while( refreshing )
    {
      if(!stream)
        stream = _popen("dir /s f:\\p\\malkia\\ftxui", "r");
      char buf[1024]{};
      if(stream)
        fgets(buf,sizeof(buf),stream);
      if(buf[0])
      {
        std::string l = buf;
        screen.Post([l, &lines]{ lines.push_back(l); });
      }
      else if(stream)
      {
        screen.Post(Event::Custom);
        _pclose(stream);
        stream = nullptr;
        refreshing = false;
      }
    }
  });
  */

  InputOption style_1 = InputOption::Default();

  InputOption style_2 = InputOption::Spacious();

  InputOption style_3 = InputOption::Spacious();
  style_3.transform = [](InputState state) {
    state.element |= borderEmpty;

    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= borderDouble;
      state.element |= bgcolor(Color::White);
      state.element |= color(Color::Black);
    } else if (state.hovered) {
      state.element |= borderRounded;
      state.element |= bgcolor(LinearGradient(90, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    } else {
      state.element |= border;
      state.element |= bgcolor(LinearGradient(0, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    }

    return state.element;
  };

  InputOption style_4;// = InputOption::Default();
  style_4.transform = [](InputState state) {
    // state.element = hbox({
    //     text("Theorem"), // | center | borderEmpty | bgcolor(Color::Red),
    //     //separatorEmpty(),
    //     //separator() | color(Color::White),
    //     //separatorEmpty(),
    //     std::move(state.element),
    // });

    //state.element |= borderEmpty;
    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= bgcolor(Color::Black);
    } else {
      state.element |= bgcolor(Color::Blue);
    }

    if (state.hovered) {
      state.element |= bgcolor(Color::GrayDark);
    }

    return state.element;//vbox({state.element, separatorEmpty()});
  };
  
  auto vc = Container::Vertical({});
  for (int i = 0; i < lines.size(); ++i) 
  {
    //vc->Add(Renderer([i, &lines]{ return hbox({text(lines[i])}) | focus;}));
     vc->Add(Input(&(lines[i]), lines[i], style_4));
  }

  // std::vector<std::string> entries = {
  //     "entry 1",
  //     "entry 2",
  //     "entry 3",
  // };
  // int selected = 0;

  // MenuOption option;
  // option.on_enter = screen.ExitLoopClosure();
  // auto menu = Menu(&entries, &selected, option);


  //std::cout << "Selected element = " << selected << std::endl;
  //refreshing = false;
  //refresh.join();

  auto renderer = Renderer(vc, [&] {
    return vc->Render() | vscroll_indicator | hscroll_indicator | frame | border | size(HEIGHT, LESS_THAN, 25) | size(WIDTH, LESS_THAN, 80);
  });

  screen.Loop(renderer);

  return 0;
}