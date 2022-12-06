import glob
import os
import re


class Library:

    @staticmethod
    def __walk_path(path: str):
        import os
        for root, dirs, files in os.walk(path):
            yield root, dirs, files

    @staticmethod
    def __list_to_string(lis: list) -> str:
        return ' '.join(str(elem) for elem in lis)

    @staticmethod
    def __filter_not_list(lis: list, regex: str) -> list:
        regex = re.compile(regex)
        return [i for i in lis if not regex.search(i)]

    @staticmethod
    def __filter_list(lis: list, regex: str) -> list:
        regex = re.compile(regex)
        return [i for i in lis if regex.search(i)]

    @staticmethod
    def __filter_header(lis: list) -> list:
        pass

    @staticmethod
    def __root_dirs_files(path: str) -> (str, list, list):
        from os import walk

        for (dirpath, dirnames, filenames) in walk(path):
            # Try to filter test, example and hidden directories
            dirnames = Library.__filter_not_list(dirnames, r'.*test.*')
            dirnames = Library.__filter_not_list(dirnames, r'.*example.*')
            dirnames = Library.__filter_not_list(dirnames, r'\..*')
            dirnames = Library.__filter_not_list(dirnames, r'.*extra.*')
            dirnames = Library.__filter_not_list(dirnames, r'.*build.*')
            dirnames = Library.__filter_not_list(dirnames, r'.*doc.*')

            for names in dirnames:
                _, _, files = Library.__root_dirs_files(os.path.join(dirpath, names))

                filenames.append(files)

            return dirpath, dirnames, filenames

    @staticmethod
    def index(path: str):
        path = path.strip(os.sep)

        root, dirs, files = Library.__root_dirs_files(path)
        headers = list()
        while dirs is not None:

            headers.append(Library.__filter_list(files, r"\.hp{,2}$"))

            root, dirs, files = Library.__root_dirs_files(path)

        return True


def __parse_args():
    import argparse

    parser = argparse.ArgumentParser(
        description='Utility for indexing libraries for CMake.',
        epilog='Hope that helps you.'
    )

    parser.add_argument('target',
                        metavar='DIRECTORY',
                        type=str,
                        help='Directory to search in'
                        )

    return parser.parse_args()


def main():
    args = __parse_args()

    directories = list()
    files = list()
    for r, ds, fs in os.walk(args.target):
        for fn in fs:
            if fn.endswith(".h"):
                files.append(os.path.join(r, fn))
                directories.append(r)

    for file in files:
        print(f"{file}")

    print("\nDirectories:\n")
    for directory in directories:
        print(f"${{CMAKE_CURRENT_SOURCE_DIR}}/{directory}")



if __name__ == '__main__':
    main()
