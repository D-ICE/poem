import unittest
import pypoem

list_of_names = [
    "AWA",
    "AWC",
    "AWS",
    "AddedWaveResistance_K",
    "AddedWaveResistance_N",
    "AddedWaveResistance_X",
    "AddedWaveResistance_Y",
    "AdvanceRatio",
    "CalmWaterResistance_X",
    "Constraint_1P_daggerboard_MaxAoA",
    "Constraint_1S_daggerboard_MaxAoA",
    "Constraint_2P_daggerboard_MaxAoA",
    "Constraint_2S_daggerboard_MaxAoA",
    "Constraint_neoliner_MaxHeelingAngle",
    "Constraint_neoliner_MaxLeewayAngle",
    "Constraint_port_MaxAdvanceRatio",
    "Constraint_port_MaxAoA",
    "Constraint_port_MaxDeflectionAngle",
    "Constraint_port_PositiveAdvanceRatio",
    "Constraint_starboard_MaxAdvanceRatio",
    "Constraint_starboard_MaxAoA",
    "Constraint_starboard_MaxDeflectionAngle",
    "Constraint_starboard_PositiveAdvanceRatio",
    "Daggerboard_1P_daggerboard_AngleOfAttack",
    "Daggerboard_1P_daggerboard_K",
    "Daggerboard_1P_daggerboard_N",
    "Daggerboard_1P_daggerboard_X",
    "Daggerboard_1P_daggerboard_Y",
    "Daggerboard_1S_daggerboard_AngleOfAttack",
    "Daggerboard_1S_daggerboard_K",
    "Daggerboard_1S_daggerboard_N",
    "Daggerboard_1S_daggerboard_X",
    "Daggerboard_1S_daggerboard_Y",
    "Daggerboard_2P_daggerboard_AngleOfAttack",
    "Daggerboard_2P_daggerboard_K",
    "Daggerboard_2P_daggerboard_N",
    "Daggerboard_2P_daggerboard_X",
    "Daggerboard_2P_daggerboard_Y",
    "Daggerboard_2S_daggerboard_AngleOfAttack",
    "Daggerboard_2S_daggerboard_K",
    "Daggerboard_2S_daggerboard_N",
    "Daggerboard_2S_daggerboard_X",
    "Daggerboard_2S_daggerboard_Y",
    "DaggerboardsDeflectionAngle",
    "EquilibriumSolverConverged",
    "EquilibriumSolverNbIter",
    "EquilibriumSolverResidue",
    "EquilibriumSolverStatus",
    "HEELING",
    "HandlePosition",
    "HasConstraintViolation",
    "HeelingRestoringMoment",
    "Hs",
    "LEEWAY",
    "Manoeuvring_K",
    "Manoeuvring_N",
    "Manoeuvring_X",
    "Manoeuvring_Y",
    "OptimizerNbIter",
    "OptimizerStatus",
    "OutOfModellingRange",
    "PitchRatio",
    "PropellerTorque",
    "QuasiPropulsiveEfficiency",
    "RPM",
    "RuddersAngleOfAttack",
    "RuddersDeflectionAngle",
    "STW",
    "Sail_solidsail_ActivePower",
    "Sail_solidsail_BendingMoment",
    "Sail_solidsail_K",
    "Sail_solidsail_N",
    "Sail_solidsail_X",
    "Sail_solidsail_Y",
    "SailsDepoweringCoeff",
    "SurgeVelocity",
    "SwayVelocity",
    "TWA",
    "TWC",
    "TWS",
    "TotalBrakePower",
    "TotalDaggerboards_K",
    "TotalDaggerboards_N",
    "TotalDaggerboards_X",
    "TotalDaggerboards_Y",
    "TotalPropellersThrust",
    "TotalPropulsion_K",
    "TotalPropulsion_N",
    "TotalPropulsion_X",
    "TotalPropulsion_Y",
    "TotalRudderSwayLoads",
    "TotalRudderYawTorque",
    "TotalRuddersSurgeLoads",
    "TotalSailsActivePower",
    "TotalSails_K",
    "TotalSails_N",
    "TotalSails_X",
    "TotalSails_Y",
    "WA",
    "WindLoads_K",
    "WindLoads_N",
    "WindLoads_X",
    "WindLoads_Y",
    "vessel_K",
    "vessel_N",
    "vessel_X",
    "vessel_Y",
]


class TestReader(unittest.TestCase):
    def test_file_exists(self):
        with self.assertRaises(pypoem.PoemError):
            _ = pypoem.Polar("../data/doesnt_exist.nc")
        try:
            _ = pypoem.Polar("../data/polar_dev.nc")
        except pypoem.PoemError:
            self.fail("Polar(path) raised NcError even though file exists.")

    def test_getnames(self):
        polar = pypoem.Polar("../data/polar_dev.nc")
        for name in polar.get_names():
            self.assertIn(name, list_of_names)

    def test_getattr(self):
        polar = pypoem.Polar("../data/polar_dev.nc")
        self.assertEqual(
            "D-ICE ENGINEERING (C)", polar.get_attributes().get("copyright")
        )


if __name__ == "__main__":
    unittest.main()
