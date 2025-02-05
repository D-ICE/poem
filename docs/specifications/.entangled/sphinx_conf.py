from importlib.metadata import version as get_version
release: str = get_version("pypoem")
# for example take major/minor
version: str = ".".join(release.split('.')[:2])
