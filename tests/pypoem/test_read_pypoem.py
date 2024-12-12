import pytest
import pypoem
import os
import boto3, botocore

TEST_DATA_BUCKET_NAME = "s3://dice-test-data"
PERF_POLARSET_PATH_S3 = "/poem/Canopee-perf-polars_20241203_18H05.nc"


@pytest.fixture
def random() -> None:
    pass


def download_test_data_from_s3(
    bucket_name: str = TEST_DATA_BUCKET_NAME,
    file_path_s3: str = PERF_POLARSET_PATH_S3,
) -> None:
    try:
        boto3.resource("s3").download_file(
            bucket_name,
            file_path_s3,
        )
    except botocore.exceptions.ClientError as e:
        if e.response["Error"]["Code"] == "404":
            print("The test data you are looking for does not exist.")
        else:
            raise


def test_load_PerformancePolarSet() -> None:

    TEST_DATA_BUCKET.download_file(".", PERF_POLARSET_PATH)

    perf_polar_set = pypoem.read_performance_polar_set(polar_file)


if __name__ == "__main__":
    random()
