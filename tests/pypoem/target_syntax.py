import poem


def main():
    filename = "polar_def.nc"

    reader = poem.PolarSetReader()
    reader.Read(filename)

    polar_set = reader.polar_set()
    attributes = reader.attributes()

    polar_names = polar_set.polar_names()
    for name in polar_names:
        print(name)


if __name__ == "__main__":
    main()
