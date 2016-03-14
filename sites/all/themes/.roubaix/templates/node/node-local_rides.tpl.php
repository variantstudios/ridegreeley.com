<?php
// $Id: node.tpl.php,v 1.4.2.1 2009/08/10 10:48:33 goba Exp $

/**
 * @file node.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<article id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> rides clear-block">
	<?php if (!$page): ?>
	  <div class="fade mosaic-block">
	    <a href="<?php print $node_url; ?>" class="mosaic-overlay">
	      <?php if($node->field_distance[0]['view'] || $node->field_elevation[0]['view'] || $node->field_type[0]['view'] || $node->field_rating[0]['view']):?>
	        <table class="details">
	          <?php if($node->field_distance[0]['view']): ?>
		          <tr>
		            <th>Distance</th>
		            <td><?php print $node->field_distance[0]['view']; ?></td>
		          </tr>
	          <?php endif; ?>
	          <?php if($node->field_elevation[0]['view']): ?>
		          <tr>
		            <th>Elevation</th>
		            <td><?php print $node->field_elevation[0]['view']; ?></td>
		          </tr>
	          <?php endif; ?>
	          <?php if($node->field_type[0]['view']): ?>
		          <tr>
		            <th>Type</th>
		            <td><?php print $node->field_type[0]['view']; ?></td>
		          </tr>
		        <?php endif; ?>
		        <?php if($node->field_rating[0]['view']): ?>
		          <tr>
		            <th>Rating</th>
		            <td><?php print $node->field_rating[0]['view']; ?></td>
		          </tr>
		        <?php endif; ?>
	        </table>
	      <?php endif; ?>
	    </a>
	    <div class="mosaic-backdrop">
	      <?php print $node->field_photos[0]['view']; ?>
	    </div>
	  </div>
	  
	  <div class="ride-description">
	    <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
	    <?php print $content; ?>
	    <a href="<?php print $node_url; ?>" class="button">Find Out More</a>
	  </div>
	<?php endif; ?>
  
  <?php if($page): ?>
    <?php print $content ?>
  <?php endif; ?>
</article>