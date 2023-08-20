#include <workspace/workspace.h>

int main() {
    wsp::workbranch br;

    br.submit<wsp::task::nor>([]{ std::cout << "task B done\n";} );

    br.submit<wsp::task::urg>( []{std::cout << "task A done\n";} );

    //等待任务完成
    br.wait_tasks();
}