            #include <atomic>

            int main (int argc, char** argv)
            {
                std::atomic<long long> ll { static_cast<long long> (argc) };
                ll ^= static_cast<long long> (ll.is_lock_free());
                return static_cast<int> (ll);
            }
        