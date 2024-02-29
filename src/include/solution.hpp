#pragma once

namespace babp {

    template<typename Task, typename ResultType>
    class Solver {

        ResultType solve(Task const& task);
    };

    class TaskHolder {
        
    };

} // namespace babp