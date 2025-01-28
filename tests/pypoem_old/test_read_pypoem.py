import pytest
import logging
import pypoem
from test_tools import download_test_data_from_s3

TEST_DATA_BUCKET_NAME = "dice-test-data"
PERF_POLARSET_NAME = "Canopee-perf-polars_20241203_18H05.nc"
TEST_DATA_DIR_S3 = "poem"
TEST_DATA_DIR_LOCAL = "tmp"


@pytest.fixture
def perf_polar_set():
    file_path = download_test_data_from_s3(
        bucket_name=TEST_DATA_BUCKET_NAME,
        file_name=PERF_POLARSET_NAME,
        dir_s3=TEST_DATA_DIR_S3,
        dir_local=TEST_DATA_DIR_LOCAL,
    )
    return pypoem.read_performance_polar_set(file_path)


def test_perf_polar_set(perf_polar_set):
    """Test that we can load the performance polar set."""
    assert perf_polar_set is not None


if __name__ == "__main__":
    pytest.main([__file__])
