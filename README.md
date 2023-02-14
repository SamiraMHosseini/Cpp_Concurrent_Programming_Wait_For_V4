# C++ Concurrent Programming
# Return value of std::condition_variable::wait_for

The unconditional version (1) returns cv_status::timeout if the function returns because rel_time has passed, or cv_status::no_timeout otherwise.
The predicate version (2) returns pred(), regardless of whether the timeout was triggered (although it can only be false if triggered).


