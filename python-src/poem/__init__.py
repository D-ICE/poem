import os
import typing

if not typing.TYPE_CHECKING and os.getenv("PYBIND11_PROJECT_PYTHON_DEBUG"):
    from ._pypoem_d import *
else:
    from ._pypoem import *
