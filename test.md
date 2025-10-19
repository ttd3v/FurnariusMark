Here's a comprehensive test markdown file to test your parser:

```markdown
# Main Title

This is a paragraph with **bold text** and *italic text* and ***bold italic***.

## Secondary Header

Here's some `inline code` in the middle of a sentence.

### Tertiary Header

Let's test lists:
- First item with *italic*
- Second item with **bold**
- Third item with `code`

---

Now testing code blocks:
```
function hello() {
    console.log("Hello world!");
}
```

And another paragraph after the code block.

# Another H1

Testing multiple bold in one line: **bold1** and **bold2** and *italic1*.

## Edge Cases

What about *italic at end*.
**Bold at end**.

Empty code: ``

# Header without space
##Another header
###Third header

- List item
-- Not a list
--- Not a list

* Not a bullet (should this be italic?)

**Bold** then normal text.

The quick brown fox jumps over the lazy dog.

# One more header

Testing `code` with **bold** and *italic* around it.

End of document.
```

## What This Tests:

1. **Headers** - All levels, with and without proper spacing
2. **Text formatting** - Bold, italic, bold-italic
3. **Code** - Inline code and code blocks
4. **Lists** - Basic unordered lists
5. **Horizontal rules** - `---`
6. **Edge cases**:
   - Formatting at line ends
   - Empty code blocks
   - Improper markdown syntax
   - Mixed formatting
   - Multiple formatting in one line
7. **Paragraph separation**
8. **Line handling**

This should reveal any issues with your state machine approach, especially around:
- State transitions between different formatting types
- Line boundary handling
- Nested formatting recognition
- Error recovery from malformed markdown

Save this as `test.md` and run it through your parser to see how it handles various scenarios!
