import pytest, os, boto3, logging
import pypoem


logging.basicConfig(
    level=logging.INFO,
    format="[%(filename)s] %(levelname)s: %(message)s",
)
logger = logging.getLogger(__name__)


def download_test_data_from_s3(
    bucket_name: str,
    file_name: str,
    dir_s3: str,
    dir_local: str = "tmp",
) -> str:
    """Download test data from S3 if it doesn't exist locally."""

    logger.info(f"Getting the test data {file_name}")

    file_path_local = os.path.join(dir_local, file_name)
    file_path_s3 = os.path.join(dir_s3, file_name)

    os.makedirs(dir_local, exist_ok=True)

    if not os.path.exists(file_path_local):
        try:
            s3 = boto3.client("s3")
            s3.download_file(bucket_name, file_path_s3, file_path_local)
        except Exception as e:
            logger.error(f"Failed to download test data: {e}")
            raise
    else:
        logger.info("The test data already exists... Download skipped")

    return file_path_local
