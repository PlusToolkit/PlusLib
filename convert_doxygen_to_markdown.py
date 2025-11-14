#!/usr/bin/env python3
"""
Convert Doxygen documentation files (.dox) to Markdown (.md) for ReadTheDocs.
This preserves the 1:1 structure of the existing documentation.
"""

import os
import re
import shutil
from pathlib import Path

def convert_doxygen_to_markdown(content):
    """
    Convert Doxygen markup to Markdown.
    """
    # Remove Doxygen comment blocks
    content = re.sub(r'/\*!', '', content)
    content = re.sub(r'\*/', '', content)
    
    # Convert \page to # heading
    content = re.sub(r'\\page\s+\w+\s+(.+?)$', r'# \1', content, flags=re.MULTILINE)
    
    # Convert \section to ## heading
    content = re.sub(r'\\section\s+\w+\s+(.+?)$', r'## \1', content, flags=re.MULTILINE)
    
    # Convert \subsection to ### heading
    content = re.sub(r'\\subsection\s+\w+\s+(.+?)$', r'### \1', content, flags=re.MULTILINE)
    
    # Convert \par to #### or bold text
    content = re.sub(r'\\par\s+(.+?)$', r'#### \1', content, flags=re.MULTILINE)
    
    # Convert \ref links
    content = re.sub(r'\\ref\s+(\w+)', r'`\1`', content)
    
    # Convert <a href> links to markdown
    def convert_link(match):
        url = match.group(1)
        text = match.group(2)
        return f'[{text}]({url})'
    content = re.sub(r'<a\s+href="([^"]+)"\s*>\s*([^<]+)</a>', convert_link, content)
    
    # Convert \c code to `code`
    content = re.sub(r'\\c\s+(\S+)', r'`\1`', content)
    
    # Convert <tt> to `code`
    content = re.sub(r'<tt>\s*([^<]+?)\s*</tt>', r'`\1`', content)
    
    # Convert \b bold
    content = re.sub(r'\\b\s+(\w+)', r'**\1**', content)
    
    # Convert XML attributes
    content = re.sub(r'\\xmlAtt\s+', r'- **', content)
    content = re.sub(r'\\xmlElem\s+', r'- **', content)
    
    # Convert \RequiredAtt and \OptionalAtt
    content = re.sub(r'\\RequiredAtt(\{[^}]*\})?', r'** (Required)', content)
    content = re.sub(r'\\OptionalAtt(\{[^}]*\})?', lambda m: f'** (Optional, default: {m.group(1)[1:-1] if m.group(1) else ""})', content)
    
    # Convert \include to code block
    def convert_include(match):
        filepath = match.group(1)
        return f'\n```xml\n<!-- File: {filepath} -->\n<!-- Content would be included here -->\n```\n'
    content = re.sub(r'\\include\s+"([^"]+)"', convert_include, content)
    
    # Clean up extra asterisks from comment lines
    content = re.sub(r'^\s*\*\s?', '', content, flags=re.MULTILINE)
    
    # Clean up multiple blank lines
    content = re.sub(r'\n{3,}', '\n\n', content)
    
    return content.strip()

def get_output_path(input_file, source_dir, output_dir):
    """
    Determine the output path for a converted file.
    """
    rel_path = input_file.relative_to(source_dir)
    # Change extension from .dox to .md
    output_path = output_dir / rel_path.with_suffix('.md')
    return output_path

def main():
    # Paths
    repo_root = Path(__file__).parent
    source_dir = repo_root / 'src' / 'Documentation' / 'UserManual'
    output_dir = repo_root / 'docs'
    
    # Create output directories
    devices_dir = output_dir / 'devices'
    algorithms_dir = output_dir / 'algorithms'
    applications_dir = output_dir / 'applications'
    file_formats_dir = output_dir / 'file-formats'
    
    for dir_path in [devices_dir, algorithms_dir, applications_dir, file_formats_dir]:
        dir_path.mkdir(parents=True, exist_ok=True)
    
    # Copy images
    images_src = source_dir
    images_dst = output_dir / 'images'
    images_dst.mkdir(exist_ok=True)
    
    # Process all .dox files
    dox_files = list(source_dir.glob('*.dox'))
    print(f"Found {len(dox_files)} .dox files to convert")
    
    for dox_file in sorted(dox_files):
        filename = dox_file.stem
        
        # Read content
        with open(dox_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Convert to markdown
        md_content = convert_doxygen_to_markdown(content)
        
        # Determine output directory based on filename prefix
        if filename.startswith('Device'):
            output_path = devices_dir / f"{filename}.md"
        elif filename.startswith('Algorithm'):
            output_path = algorithms_dir / f"{filename}.md"
        elif filename.startswith('Application'):
            output_path = applications_dir / f"{filename}.md"
        elif filename.startswith('File'):
            output_path = file_formats_dir / f"{filename}.md"
        else:
            # Other files go to root docs
            output_path = output_dir / f"{filename}.md"
        
        # Write markdown file
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(md_content)
        
        print(f"Converted: {dox_file.name} -> {output_path.relative_to(output_dir)}")
    
    # Copy image files
    image_extensions = ['.png', '.jpg', '.jpeg', '.gif', '.emf']
    for img_file in source_dir.iterdir():
        if img_file.suffix.lower() in image_extensions:
            shutil.copy2(img_file, images_dst / img_file.name)
            print(f"Copied image: {img_file.name}")
    
    print(f"\nConversion complete!")
    print(f"Devices: {len(list(devices_dir.glob('*.md')))} files")
    print(f"Algorithms: {len(list(algorithms_dir.glob('*.md')))} files")
    print(f"Applications: {len(list(applications_dir.glob('*.md')))} files")
    print(f"File Formats: {len(list(file_formats_dir.glob('*.md')))} files")

if __name__ == '__main__':
    main()
