            #include <atomic>

            int main (int argc, char** argv)
            {
                std::atomic<long long> ll { static_cast<long long> (argc) };
                ll ^= 1;
                return static_cast<int> (ll);
            }
        