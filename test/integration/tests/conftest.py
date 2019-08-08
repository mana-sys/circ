def pytest_addoption(parser):
    parser.addoption(
        "--circ",
        action="append",
        default=[],
        help="path to the circ_sv executable"
    )


def pytest_generate_tests(metafunc):
    if "circ_sv_exe" in metafunc.fixturenames:
        metafunc.parametrize("circ_sv_exe", metafunc.config.getoption("circ"))
