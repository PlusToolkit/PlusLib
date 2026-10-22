#!/usr/bin/env python3
"""Print the table of the devices that the installers of a release support.

The device list of an installer is the output of the PlusVersion tool of the
package, which the build workflow runs from the staged install tree and saves
as SupportedDevices.txt next to the installer in its artifact. The lists of all
the installers on the release are kept in supported-devices.json, an asset of
the release, so that a package that is not rebuilt keeps its list.

The table is printed as Markdown, to be added to the release notes. Requires
the gh CLI.
"""

import argparse
import glob
import json
import os
import re
import subprocess
import sys
import tempfile

# PlusApp-<version>.<revision>-<package>.exe
INSTALLER_NAME = re.compile(r'PlusApp-([0-9.]+)\.\d+-(.*)\.exe')
# Software version: Plus-2.9.0.8b7d3d48 - Win64
SOFTWARE_VERSION = re.compile(r'Software version:\s*(.*\S)')
#   - NDITracker (ver: Plus-2.9.0)
DEVICE = re.compile(r'\s*-\s+(\S+)\s+\(ver:\s*(.*?)\s*\)\s*$')
DEVICES_FILE = 'SupportedDevices.txt'


def gh(*args):
  return subprocess.run(['gh', *args], check=True, text=True, stdout=subprocess.PIPE).stdout


def parse_devices(text):
  """The software version and the {device: SDK version} of a PlusVersion output."""
  version, devices = '', {}
  for line in text.splitlines():
    m = SOFTWARE_VERSION.fullmatch(line.strip())
    if m:
      version = m[1]
      continue
    m = DEVICE.fullmatch(line)
    if m:
      devices[m[1]] = m[2]
  return version, devices


def package_name(installer):
  m = INSTALLER_NAME.fullmatch(installer)
  if not m:
    sys.exit(f'Unexpected installer name: {installer}')
  return m[2]


def package_order(package):
  """Win64 before Win32, the generic package before the editions."""
  edition, _, arch = package.rpartition('-')
  return (arch != 'Win64', arch, edition != '', edition.lower())


def read_artifacts(dirs, state):
  """Update the state with the device lists found next to the installers."""
  for dir in dirs:
    installers = glob.glob(os.path.join(dir, '*.exe'))
    if not installers:
      sys.exit(f'{dir} contains no installer')
    installer = os.path.basename(installers[0])
    package = package_name(installer)
    path = os.path.join(dir, DEVICES_FILE)
    if not os.path.exists(path):
      # PlusVersion could not be run; the list of the previous build stays.
      print(f'{package}: no {DEVICES_FILE}, keeping the previous list', file=sys.stderr)
      continue
    with open(path, encoding='utf-8-sig') as f:
      version, devices = parse_devices(f.read())
    if not devices:
      sys.exit(f'{path} lists no devices')
    state[package] = {'installer': installer, 'version': version, 'devices': devices}
    print(f'{package}: {len(devices)} devices ({version})', file=sys.stderr)


def render(state, packages):
  """The Markdown table of the devices of each package.

  The devices of every package are listed once, folded; each package is then
  described by what it adds, relative to the generic package of its architecture.
  """
  names = lambda devices: ', '.join(f'`{d}`' for d in sorted(devices, key=str.lower))
  known = [p for p in packages if p in state]
  sets = {p: set(state[p]['devices']) for p in known}
  common = set.intersection(*sets.values()) if known else set()

  lines = ['### Supported devices', '']
  if common:
    lines += [f'<details><summary>In every installer: {len(common)} device type{"s" * (len(common) != 1)}</summary>', '', names(common) + '.', '', '</details>', '']
  lines += ['| Installer | Additional devices |', '|---|---|']
  for p in packages:
    if p not in state:
      cell = '*not known: the device list of the installer could not be generated*'
    else:
      # An edition extends the generic package of its architecture.
      base = p.rpartition('-')[2]
      if not (base != p and base in sets and sets[base] < sets[p] and sets[base] > common):
        base = None
      extra = sets[p] - (sets[base] if base else common)
      cell = (f'{base} + ' if base else '') + (names(extra) if extra else '')
      cell = cell.rstrip(' +') or '&ndash;'
    lines.append(f'| {p} | {cell} |')
  lines += ['', 'Device types as named in the configuration files, listed by the `PlusVersion.exe` of each installer.', '']
  return chr(10).join(lines)


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument('--repo', default=os.environ.get('GH_REPO'), help='OWNER/REPO of the release (default: $GH_REPO)')
  parser.add_argument('--release', required=True, metavar='TAG', help='tag of the release')
  parser.add_argument('--artifacts', nargs='*', default=[], metavar='DIR',
                      help=f'directories that hold one installer each and, if PlusVersion ran, its {DEVICES_FILE}')
  parser.add_argument('--state', default='supported-devices.json',
                      help='name of the release asset that keeps the device lists (default: %(default)s)')
  parser.add_argument('--no-upload', action='store_true', help='do not update the release asset')
  args = parser.parse_args()
  if not args.repo:
    sys.exit('No repository: pass --repo or set GH_REPO')
  release = ['release', 'view', args.release, '--repo', args.repo]
  # The table is UTF-8 whatever the console encoding (the check marks).
  sys.stdout.reconfigure(encoding='utf-8')

  with tempfile.TemporaryDirectory() as tmp:
    path = os.path.join(tmp, args.state)
    state = {}
    if gh(*release, '--json', 'assets', '--jq', f'.assets[] | select(.name == "{args.state}") | .name'):
      gh('release', 'download', args.release, '--repo', args.repo, '--pattern', args.state, '--dir', tmp)
      with open(path, encoding='utf-8') as f:
        state = json.load(f)

    read_artifacts(args.artifacts, state)

    # The packages are the installers on the release, which the caller has
    # already updated; the lists of installers that were removed go with them.
    installers = gh(*release, '--json', 'assets', '--jq', '.assets[].name | select(endswith(".exe"))').split()
    packages = sorted({package_name(i) for i in installers}, key=package_order)
    state = {p: state[p] for p in packages if p in state}

    with open(path, 'w', encoding='utf-8') as f:
      json.dump(state, f, indent=2, sort_keys=True)
      f.write('\n')
    if not args.no_upload:
      gh('release', 'upload', args.release, '--repo', args.repo, path, '--clobber')
      print(f'Uploaded {args.state}: {len(state)} of {len(packages)} packages', file=sys.stderr)

  print(render(state, packages))


if __name__ == '__main__':
  main()
