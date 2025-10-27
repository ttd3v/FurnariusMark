A software that parses markdown files into HTML, it is meant to help automate the process of generating a good website for your projects. I intended to make it for generating documentation, but it is a tool and can be used however the user wants.

The software is written in C, so it can be as lightweight as possible, and easy to install — Compile, good to go. It does have dependencies (argp.h, asm-generic/errno.h, stdio.h, stdlib.h, and string.h), but they can be considered standard library and likely are already on your machine.

Installing the software is straightforward; you either download the binary through the 
<a href="https://github.com/ttd3v/FurnariusMark/releases/download/0.0.1/FurnariusMark">latest release binary</a> or compile it yourself. With the binary, place it at `/usr/local/bin`.

Make sure you apply the needed permissions, using `chmod +x`. Also, unfortunately, you likely won't be able to install directly from the browser. It might not have permissions to write into `/usr/local/bin`, you will have to move the binary there by yourself.

If you don't like having to write "FurnariusMark" every time you want to use the tool, rename the binary name to whatever you want. If you want to write "poop" to use the tool, name the binary that way, and it will be called by this name.

# Documentation - 0.1.0
The project is in its first version (0.1.0), and it has the markdown essentials support, such as bold, italic, code (inline, multiline), headers, and line breakers (aka &lt;<br/>&gt;). It still lacks table, links (URLs and images), and citation quotes. It likely missed some features of markdown, since I made the tool tuned for how I interact with markdown. Yet, if you feel something is off, feel free to raise an issue; your help to improve the project would be appreciated (As it is under the GPL-3.0 license, you can raise a PR too). 

## HTML tag
As this project is meant to help with the construction of websites from markdown, having a way of generating raw HTML can be useful. That's why you can do so, by writing "::html::", once for starting the HTML context and once for closing this context. Everything within those tags will be directly pushed to the HTML file.
*Note: The HTML tag has to be at the start of a line for it to work, like a header tag (#) has.*

### Skip HTML Tag
Everything will be in HTML. If you write a tag as is, it will be interpreted by the browser. The tag is a guarantee that the parser will ignore the line.

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

## Uncomfortable question
"Could I never use this project and write into an HTML file directly?", that would be answered with "yes," of course. I made the tool because I would rather write markdown instead of HTML, since I find it more convenient. But if you find the tool weird or unnecessary, spit at it and move on.

