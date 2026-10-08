#!/usr/bin/env python3
"""Record the download counts of the installers of GitHub releases.

The counts are kept in CSV files stored as assets of a release, by default
<release>.csv on the download-stats release of PlusToolkit/PlusLibData: one row
per date, one column per package (e.g. 2.9.0-Win64), holding the total number
of downloads of the package up to that date. Columns are added as packages
appear, and every file has the same meaning, so that the files of several
releases can be combined on the date.

For a release that keeps its installers, the total is the download count of
the installer; by default every release that is not a pre-release is recorded.
For installers that are replaced (the nightly builds on the pre-release), pass
--release and --accumulate: the download count of each installer is then added
to the total of the previous row, and should be recorded once, right before
the installer is removed.

Nothing is written when no total changed since the last row.

Requires the gh CLI. The ledger release is accessed with the token in
LEDGER_TOKEN when it is set, otherwise with the same token as the releases.
"""

import argparse
import csv
import datetime
import os
import re
import subprocess
import sys
import tempfile

# PlusApp-<version>.<revision>-<package>.exe, where the revision is the build
# date (20261005) or, on old releases, a Subversion revision (5073).
INSTALLER_NAME = re.compile(r'PlusApp-([0-9.]+)\.\d+-(.*)\.exe')


def gh(*args, token=None):
  env = dict(os.environ, GH_TOKEN=token) if token else None
  return subprocess.run(['gh', *args], check=True, text=True, stdout=subprocess.PIPE, env=env).stdout


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument('--repo', default=os.environ.get('GH_REPO'), help='OWNER/REPO of the releases (default: $GH_REPO)')
  parser.add_argument('--release', action='append', metavar='TAG',
                      help='tag of a release whose installers are counted (default: all releases but pre-releases)')
  parser.add_argument('--ledger-repo', default='PlusToolkit/PlusLibData', help='OWNER/REPO of the ledger release (default: %(default)s)')
  parser.add_argument('--ledger-release', default='download-stats', help='tag of the release that holds the CSV files (default: %(default)s)')
  parser.add_argument('--file', help='name of the CSV asset, for a single release (default: <release>.csv)')
  parser.add_argument('--accumulate', action='store_true',
                      help='add the counts to the totals of the previous row (installers that are replaced)')
  parser.add_argument('--installer', nargs='*', metavar='NAME',
                      help='record only these installers (default: all of the release)')
  args = parser.parse_args()
  if not args.repo:
    sys.exit('No repository: pass --repo or set GH_REPO')
  if (args.file or args.accumulate or args.installer is not None) and len(args.release or []) != 1:
    parser.error('--file, --accumulate and --installer apply to a single --release')
  releases = args.release or gh('release', 'list', '--repo', args.repo, '--exclude-pre-releases', '--exclude-drafts',
                                '--json', 'tagName', '--jq', '.[].tagName').split()
  for release in releases:
    record(args, release)


def record(args, release):
  file = args.file or f'{release}.csv'
  ledger = ['--repo', args.ledger_repo, args.ledger_release]
  token = os.environ.get('LEDGER_TOKEN')

  counts = gh('release', 'view', release, '--repo', args.repo, '--json', 'assets',
              '--jq', '.assets[] | select(.name | endswith(".exe")) | "\\(.name) \\(.downloadCount)"')
  if args.installer is not None:
    counts = '\n'.join(line for line in counts.splitlines() if line.split()[0] in args.installer)
  if not counts:
    print(f'No installers to record for {release}')
    return

  header, table = ['date'], {}
  with tempfile.TemporaryDirectory() as tmp:
    path = os.path.join(tmp, file)
    if gh('release', 'view', *ledger, '--json', 'assets', '--jq', f'.assets[] | select(.name == "{file}") | .name', token=token):
      gh('release', 'download', *ledger, '--pattern', file, '--dir', tmp, token=token)
      with open(path, newline='') as f:
        rows = list(csv.reader(f))
      header = rows[0]
      table = {row[0]: dict(zip(header[1:], row[1:])) for row in rows[1:]}

    today = datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%d')
    last = table[max(table)] if table else {}
    # Today's row starts from the last one, so that a package not counted
    # today keeps its total.
    row = table.setdefault(today, dict(last))
    changed = False
    for line in counts.splitlines():
      name, count = line.split()
      m = INSTALLER_NAME.fullmatch(name)
      if not m:
        sys.exit(f'Unexpected installer name: {name}')
      package = f'{m[1]}-{m[2]}'
      if package not in header:
        header.append(package)
      total = int(row.get(package) or 0) + int(count) if args.accumulate else int(count)
      changed |= str(total) != last.get(package)
      row[package] = str(total)
      print(f'{package}: {count} -> {total}')
    if not changed:
      print(f'Nothing new for {file}')
      return

    with open(path, 'w', newline='') as f:
      writer = csv.writer(f, lineterminator='\n')
      writer.writerow(header)
      for date in sorted(table):
        writer.writerow([date] + [table[date].get(p, '') for p in header[1:]])
    gh('release', 'upload', *ledger, path, '--clobber', token=token)
    print(f'Uploaded {file} to {args.ledger_repo} {args.ledger_release}: {len(table)} rows, {len(header) - 1} packages')


if __name__ == '__main__':
  main()
