A software that parses markdown files into HTML, it is meant to help automate the process of generating a good website for your projects. I intended to make it for generating documentation, but it is a tool and can be used however the user wants.

The software is written in C, so it can be as lightweight as possible, and easy to install — Compile, good to go. It does have dependencies (argp.h, asm-generic/errno.h, stdio.h, stdlib.h, and string.h), but they can be considered standard library and likely are already on your machine.

Installing the software is straightforward; you either download the bin through the ::html::<a href="https://github.com/ttd3v/FurnariusMark/releases/download/0.0.1/FurnariusMark">latest release binary</a>::html:: or compile it yourself. With the binary, place it at `/usr/local/bin`; 

# Documentation - 0.0.1
The project is in its first version (0.0.1), it has the markdown essentials support such as bold, italic, code (inline, multiline), headers, and line breakers (\\n, aka <br/>). It still lacks table, links (URLs and images), and citation quotes. It likely missed some features of markdown, since I made the tool tuned for how I interact with markdown. Yet, if you feel something is off, feel free to raise an issue; your help to improve the project would be appreciated (As it is under the GPL-3.0 license, you can raise a PR too). 

## HTML tag
As this project is meant to help with the construction of websites from markdown, having a way of generating raw HTML can be useful. That's why you can do so, by writing "::html::", once for starting the HTML context and once for closing this context. Everything within those tags will be directly pushed to the HTML file.

## Usage
You use the CLI for parsing your markdown files. If you just enter the CLI trigger, a prompt will be raised. If you enter the flags, you will be able to skip it and generate the result directly.

### CLI Flags
::html::
<table>
  <thead>
    <tr>
      <th>Short</th>
      <th>Long</th>
      <th>Argument</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>-f</td>
      <td>--file</td>
      <td>FILE</td>
      <td>Input markdown file path</td>
    </tr>
    <tr>
      <td>-t</td>
      <td>--title</td>
      <td>TITLE</td>
      <td>HTML page title</td>
    </tr>
    <tr>
      <td>-c</td>
      <td>--css</td>
      <td>CSS_HREF</td>
      <td>CSS stylesheet href (optional)</td>
    </tr>
    <tr>
      <td>-p</td>
      <td>--preset</td>
      <td>HTML_PRESET</td>
      <td>Custom HTML preset file path</td>
    </tr>
  </tbody>
</table>
::html::


## Logic
The program has three main elements: the HTML preset, title, content, and CSS href. Within the code, you can see an HTML preset. This exists to help templating; there are three "%s" within. Where the first is replaced with the input title (Which is the page title), the second with the CSS href, and the third with the generated HTML.

## Contribution
You can do a pull request with changes you think could help the project, and you can raise an issue to help me.

## Compatibility
The code itself relies on packages that can be considered standard library; even so, it is tied to Linux.

