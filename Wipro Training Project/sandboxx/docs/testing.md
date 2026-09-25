# SandBoxX Verification & Testing Report

## Automated Test Suites (`ctest`)

1. **`namespace_test`**: Verifies generation of Linux namespace clone/unshare flags.
2. **`cgroup_test`**: Verifies cgroup directory creation and limit string writing.
3. **`filesystem_test`**: Validates rootfs directory tree setup (`proc`, `dev`, `tmp`).
4. **`security_test`**: Tests execution of capability dropping and `PR_SET_NO_NEW_PRIVS`.
5. **`runtime_test`**: Tests full lifecycle sandbox execution and clean exit code propagation.
