import os
import typing

if not typing.TYPE_CHECKING and os.getenv("PYBIND11_PROJECT_PYTHON_DEBUG"):
    from ._pypoem_d import *
    from ._pypoem_d import __version__
else:
    from ._pypoem import *
    from ._pypoem import __version__
